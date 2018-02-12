#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <map>
#include <cctype>
#include <cstdlib>
#include <memory>
#include "tinyxml2.h"
#include "expression-tree-compiler.hpp"
#include "varlength.hpp"

using namespace std;
using namespace tinyxml2;

struct LangString final
{
    string string;
    map<size_t,size_t> vcategoryAssociations;
};

struct Formatter final
{
    struct Specifier
    {
        size_t byteLocation;
        uint8_t type;
        string specifier1, specifier2;
        size_t term;
    };
    
    string modelString;
    vector<Specifier> specifiers;
};

struct PluralForm final
{
    vector<pair<string,unique_ptr<ExpressionTree>>> pluralUnits;
};

struct VariantForm final
{
    set<string> variantTypes;
};

struct Pterm final
{
    string category;
    map<string,string> variants;
};

struct Vterm final
{
    struct Variant
    {
        vector<string> types;
        string string;
    };
    
    vector<string> categories;
    vector<Variant> variants;
};

struct Pvterm final
{
    struct Variant
    {
        string ptype;
        vector<string> vtypes;
        string string;
    };
    
    string pcategory;
    vector<string> vcategories;
    vector<Variant> variants;
};

bool isASCII(const char* str)
{
    for (auto c = str; *c; c++)
        if (*c > 127) return false;
    
    return true;
}

bool isValidName(const char* str, bool spacesAllowed = false, char delimiter = 0)
{
    for (auto c = str; *c != delimiter; c++)
        if (!((*c >= 'A' && *c <= 'Z') || 
              (*c >= 'a' && *c <= 'z') || 
              (*c >= '0' && *c <= '9') || 
               *c == '-' || (spacesAllowed && *c == ' ')))
            return false;
            
    return true;
}

bool isValidName(const char* str, const char* end, bool spacesAllowed = false)
{
    for (auto c = str; c != end; c++)
    for (auto c = str; c != end; c++)
        if (!((*c >= 'A' && *c <= 'Z') || 
              (*c >= 'a' && *c <= 'z') || 
              (*c >= '0' && *c <= '9') || 
               *c == '-' || (spacesAllowed && *c == ' ')))
            return false;
            
    return true;
}

bool isSpace(char c)
{
    return c == ' ' || (c >= 0x9 && c <= 0xD);
}

const char* pickValidAttrs(XMLHandle handle, const char* objName, const char* attrName, std::string innerName, bool compound = false)
{
    auto attr = handle.ToElement()->Attribute(attrName);
    if (!attr)
    {
        cout << objName << innerName << " must have a " << attrName << "!" << endl;
        return nullptr;
    }
    if (!isValidName(attr, compound))
    {
        cout << objName << ' ' << attrName << innerName;
        cout << " must consist of letters, digits";
        cout << (compound ? ", hyphens or spaces!" : " or hyphens!") << endl;
        return nullptr;
    }
    return attr;
}

const char* pickValidAttr(XMLHandle handle, const char* objName, const char* attrName, bool compound = false)
{
    return pickValidAttrs(handle, objName, attrName, std::string(""), compound);
}

void write_str(ostream& out, const char* str)
{
    size_t s = strlen(str);
    write_varlength(out, s);
    out.write(str, s * sizeof(char));
}

void write_str(ostream& out, const std::string& str)
{
    write_varlength(out, str.size());
    out.write(str.data(), str.size() * sizeof(char));
}

bool parseLanguage(XMLDocument& doc, string inFile, map<string,PluralForm>& pluralForms,
    map<string,VariantForm>& variantForms, map<string,Pterm>& pterms, map<string,Vterm>& vterms,
    map<string,Pvterm>& pvterms, map<string,LangString>& strings, map<string,Formatter>& formatters)
{
    XMLHandle docHandle(doc);
    auto language = docHandle.FirstChildElement("language");
    
    for (auto importer = language.FirstChildElement("import-from"); importer.ToElement();
        importer = importer.NextSiblingElement("import-from"))
    {
        string file = importer.ToElement()->Attribute("file");

        auto pos = inFile.find_last_of("\\/");
        if (pos != string::npos)
            file = inFile.substr(0, pos+1) + file;

        XMLDocument subDoc;
        if (subDoc.LoadFile(file.c_str()) != XML_SUCCESS)
        {
            cout << "Error while trying to read file " << file;
            cout << " requested by " << inFile << ": " << subDoc.ErrorName() << "." << endl;
            return false;
        }
        
        if (!parseLanguage(subDoc, file, pluralForms, variantForms, pterms, vterms, pvterms, strings, formatters))
        {
            cout << "Error while loading imported file " << file << " requested by " << inFile << "." << endl;
            return false;
        }
    }
    
    for (auto pform = language.FirstChildElement("plural-forms"); pform.ToElement();
        pform = pform.NextSiblingElement("plural-forms"))
    {
        auto category = pform.ToElement()->Attribute("category");
        if (!category) category = "default";
        if (!isValidName(category))
        {
            cout << "Plural form category must consist of letters, digits or hyphens!" << endl;
            return false;
        }
        
        PluralForm pluralForm;
        for (auto pf = pform.FirstChildElement("plural-form"); pf.ToElement();
            pf = pf.NextSiblingElement("plural-form"))
        {
            auto type = pickValidAttrs(pf, "Plural form", "type", std::string(" defined on category ") + category);
            if (!type) return false;
            
            for (const auto& p : pluralForm.pluralUnits)
            {
                if (p.first == type)
                {
                    cout << "Multiple definition of plural form of type " << type;
                    cout << " defined on category " << category << '!' << endl;
                    return false;
                }
            }
            
            if (pf.FirstChild().ToNode() != nullptr)
            {
                auto text = pf.ToElement()->GetText();
                if (!text || pf.FirstChild().ToNode() != pf.LastChild().ToNode())
                {
                    cout << "Plural form of type " << type << " defined on category " << category;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return false;
                }
                
                try
                {
                    pluralForm.pluralUnits.emplace_back(type, compileExpression(text));
                }
                catch (const runtime_error &err)
                {
                    cout << "Invalid expression on plural form of type " << type << " on category " << category;
                    cout << ": " << err.what() << endl;
                    return false;
                }
            }
            else pluralForm.pluralUnits.emplace_back(type, nullptr);
        }
        
        if (!pluralForms.emplace(category, move(pluralForm)).second)
        {
            cout << "Multiple definition of plural form of category " << category << '!' << endl;
            return false;
        }
    }
    
    for (auto vform = language.FirstChildElement("variant-forms"); vform.ToElement();
        vform = vform.NextSiblingElement("variant-forms"))
    {
        auto category = pickValidAttr(vform, "Variant form", "category");
        if (!category) return false;
        
        VariantForm variantForm;
        for (auto vf = vform.FirstChildElement("variant-form"); vf.ToElement();
            vf = vf.NextSiblingElement("variant-form"))
        {
            auto type = pickValidAttrs(vf, "Variant form", "type", std::string(" defined on category ") + category);
            if (!type) return false;
            
            if (vf.FirstChild().ToNode() != nullptr)
            {
                cout << "Variant form of type " << type << " on category " << category << " must have no children!";
                return false;
            }
            
            if (!variantForm.variantTypes.emplace(type).second)
            {
                cout << "Multiple definition of variant form of type " << type;
                cout << " defined on category " << category << '!' << endl;
                return false;
            }
        }
        
        if (!variantForms.emplace(category, variantForm).second)
        {
            cout << "Multiple definition of variant form of category " << category << '!' << endl;
            return false;
        }
    }
    
    for (auto pterm = language.FirstChildElement("pterm"); pterm.ToElement();
        pterm = pterm.NextSiblingElement("pterm"))
    {
        auto name = pickValidAttr(pterm, "Pterm", "name");
        if (!name) return false;

        auto category = pterm.ToElement()->Attribute("category");
        if (!category) category = "default";
        if (!isValidName(category))
        {
            cout << "Pterm category must consist of letters, digits or hyphens!" << endl;
            return false;
        }
        
        auto pluralFormIt = pluralForms.find(category);
        if (pluralFormIt == pluralForms.end())
        {
            cout << "There is no plural form of category " << category;
            cout << " (of pterm of name " << name << ")!" << endl;
            return false;
        }
        
        Pterm ptermV;
        ptermV.category = category;
        
        for (auto variant = pterm.FirstChildElement("variant"); variant.ToElement();
            variant = variant.NextSiblingElement("variant"))
        {
            auto type = pickValidAttrs(variant, "Variant", "type", std::string(" defined on pterm ") + name);
            if (!type) return false;
            
            auto typeIt = find_if(pluralFormIt->second.pluralUnits.begin(), pluralFormIt->second.pluralUnits.end(),
                [=] (const auto& p) { return p.first == type; });
            if (typeIt == pluralFormIt->second.pluralUnits.end())
            {
                cout << "There is no plural form of type " << type << " of category " << category;
                cout << " (of pterm variant of name " << name << ")!" << endl;
                return false;
            }
            
            if (variant.FirstChild().ToNode() != nullptr)
            {
                auto text = variant.ToElement()->GetText();
                if (!text || variant.FirstChild().ToNode() != variant.LastChild().ToNode())
                {
                    cout << "Variant of type " << type << " defined on pterm " << name;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return false;
                }
                
                if (!ptermV.variants.emplace(type, text).second)
                {
                    cout << "Multiple definition of variant type " << type << " on pterm " << name << '!' << endl;
                    return false;
                }
            }
            else
            {
                cout << "Variant of type " << type << " defined on pterm " << name;
                cout << " must have some text!" << endl;
                return false;
            }
        }
        
        if (pluralFormIt->second.pluralUnits.size() != ptermV.variants.size())
        {
            cout << "Pterm " << name << " must define all types of plural form category " << category << '!' << endl;
            return false;
        }
        
        if (!pterms.emplace(name, ptermV).second)
        {
            cout << "Multiple definition of pterm " << name << "!" << endl;
            return false;
        }
    }
    
    for (auto vterm = language.FirstChildElement("vterm"); vterm.ToElement();
        vterm = vterm.NextSiblingElement("vterm"))
    {
        auto name = pickValidAttr(vterm, "Vterm", "name");
        if (!name) return false;
        
        auto category = pickValidAttr(vterm, "Vterm", "category", true);
        if (!category) return false;
        
        Vterm vtermV;
        vector<map<string,VariantForm>::const_iterator> catIterators;
        auto cur = category;
        while (*cur)
        {
            while (isSpace(*cur)) cur++; if (!*cur) break;
            auto beg = cur;
            while (*cur && !isSpace(*cur)) cur++;
            
            std::string catName(beg, cur);
            auto catIt = variantForms.find(catName);
            
            if (catIt == variantForms.end())
            {
                cout << "There is no variant form of category " << catName;
                cout << " (defined in vterm " << name << ")!" << endl;
                return false;
            }
            catIterators.push_back(catIt);
            vtermV.categories.push_back(catName);
        }
        
        if (catIterators.empty())
        {
            cout << "Vterm must have a category!" << endl;
            return false;
        }
        
        for (auto variant = vterm.FirstChildElement("variant"); variant.ToElement();
            variant = variant.NextSiblingElement("variant"))
        {
            Vterm::Variant var;
            
            auto type = pickValidAttrs(variant, "Variant", "type", std::string(" defined on vterm ") + name, true);
            if (!type) return false;
            
            auto cur = type;
            for (const auto& catIt : catIterators)
            {
                while (isSpace(*cur)) cur++; if (!*cur) break;
                auto beg = cur;
                while (*cur && !isSpace(*cur)) cur++;
                
                std::string typeName(beg, cur);
                auto typeIt = catIt->second.variantTypes.find(typeName);
                if (typeIt == catIt->second.variantTypes.end())
                {
                    cout << "There is no variant form of type " << typeName << " of category " << catIt->first;
                    cout << " (of vterm variant of name " << name << ")!" << endl;
                    return false;
                }
                
                var.types.push_back(typeName);
            }
            
            if (var.types.size() != catIterators.size())
            {
                cout << "Variant defined on vterm " << name;
                cout << " must provide variant types for all associated categories!" << endl;
                return false;
            }
            
            for (const auto& other : vtermV.variants)
            {
                if (var.types == other.types)
                {
                    cout << "Multiple definition of variant type set " << type << " on vterm " << name << '!' << endl;
                    return false;
                }
            }
            
            if (variant.FirstChild().ToNode() != nullptr)
            {
                auto text = variant.ToElement()->GetText();
                if (!text || variant.FirstChild().ToNode() != variant.LastChild().ToNode())
                {
                    cout << "Variant of type " << type << " defined on vterm " << name;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return false;
                }
                
                var.string = text;
                vtermV.variants.push_back(move(var));
            }
            else
            {
                cout << "Variant of type " << type << " defined on vterm " << name;
                cout << " must have some text!" << endl;
                return false;
            }
        }
        
        size_t variantTotals = 1;
        for (const auto& catIt : catIterators)
            variantTotals *= catIt->second.variantTypes.size();
        
        if (vtermV.variants.size() != variantTotals)
        {
            cout << "Vterm " << name << " must define all types of variant form categories " << category << '!' << endl;
            return false;
        }
        
        if (!vterms.emplace(name, vtermV).second)
        {
            cout << "Multiple definition of vterm " << name << "!" << endl;
            return false;
        }
    }
    
    for (auto pvterm = language.FirstChildElement("pvterm"); pvterm.ToElement();
        pvterm = pvterm.NextSiblingElement("pvterm"))
    {
        auto name = pickValidAttr(pvterm, "Pvterm", "name");
        if (!name) return false;
        
        auto pcategory = pvterm.ToElement()->Attribute("pcategory");
        if (!pcategory) pcategory = "default";
        if (!isValidName(pcategory))
        {
            cout << "Pterm pcategory must consist of letters, digits or hyphens!" << endl;
            return false;
        }
        
        auto pluralFormIt = pluralForms.find(pcategory);
        if (pluralFormIt == pluralForms.end())
        {
            cout << "There is no plural form of category " << pcategory;
            cout << " (of pterm of name " << name << ")!" << endl;
            return false;
        }
        
        auto vcategory = pickValidAttr(pvterm, "Pvterm", "vcategory", true);
        if (!vcategory) return false;
        
        Pvterm pvtermV;
        pvtermV.pcategory = pcategory;
        vector<map<string,VariantForm>::const_iterator> catIterators;
        auto cur = vcategory;
        while (*cur)
        {
            while (isSpace(*cur)) cur++; if (!*cur) break;
            auto beg = cur;
            while (*cur && !isSpace(*cur)) cur++;
            
            std::string catName(beg, cur);
            auto catIt = variantForms.find(catName);
            
            if (catIt == variantForms.end())
            {
                cout << "There is no variant form of category " << catName;
                cout << " (defined in pvterm " << name << ")!" << endl;
                return false;
            }
            catIterators.push_back(catIt);
            pvtermV.vcategories.push_back(catName);
        }
        
        if (catIterators.empty())
        {
            cout << "Pvterm must have a vcategory!" << endl;
            return false;
        }

        for (auto variant = pvterm.FirstChildElement("variant"); variant.ToElement();
            variant = variant.NextSiblingElement("variant"))
        {
            Pvterm::Variant var;
            
            auto type = pickValidAttrs(variant, "Variant", "type", std::string(" defined on pvterm ") + name, true);
            if (!type) return false;
            
            auto cur = type;
            {
                while (isSpace(*cur)) cur++; if (!*cur) break;
                auto beg = cur;
                while (*cur && !isSpace(*cur)) cur++;
                
                std::string typeName(beg, cur);
                auto typeIt = find_if(pluralFormIt->second.pluralUnits.begin(), pluralFormIt->second.pluralUnits.end(),
                    [=] (const auto& p) { return p.first == typeName; });
                if (typeIt == pluralFormIt->second.pluralUnits.end())
                {
                    cout << "There is no plural form of type " << typeName << " of category " << pcategory;
                    cout << " (of pvterm variant of name " << name << ")!" << endl;
                    return false;
                }
                
                var.ptype = typeName;
            }
            
            for (const auto& catIt : catIterators)
            {
                while (isSpace(*cur)) cur++; if (!*cur) break;
                auto beg = cur;
                while (*cur && !isSpace(*cur)) cur++;
                
                std::string typeName(beg, cur);
                auto typeIt = catIt->second.variantTypes.find(typeName);
                if (typeIt == catIt->second.variantTypes.end())
                {
                    cout << "There is no variant form of type " << typeName << " of category " << catIt->first;
                    cout << " (of pvterm variant of name " << name << ")!" << endl;
                    return false;
                }
                
                var.vtypes.push_back(typeName);
            }
            
            if (var.vtypes.size() != catIterators.size())
            {
                cout << "Variant defined on pvterm " << name;
                cout << " must provide variant types for all associated vcategories!" << endl;
                return false;
            }
            
            for (const auto& other : pvtermV.variants)
            {
                if (var.ptype == other.ptype && var.vtypes == other.vtypes)
                {
                    cout << "Multiple definition of variant type set " << type << " on pvterm " << name << '!' << endl;
                    return false;
                }
            }
            
            if (variant.FirstChild().ToNode() != nullptr)
            {
                auto text = variant.ToElement()->GetText();
                if (!text || variant.FirstChild().ToNode() != variant.LastChild().ToNode())
                {
                    cout << "Variant of type " << type << " defined on pvterm " << name;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return false;
                }
                
                var.string = text;
                pvtermV.variants.push_back(move(var));
            }
            else
            {
                cout << "Variant of type " << type << " defined on pvterm " << name;
                cout << " must have some text!" << endl;
                return false;
            }
        }
        
        size_t variantTotals = pluralFormIt->second.pluralUnits.size();
        for (const auto& catIt : catIterators)
            variantTotals *= catIt->second.variantTypes.size();
        
        if (pvtermV.variants.size() != variantTotals)
        {
            cout << "Pvterm " << name << " must define all types of plural form category " << pcategory;
            cout << " and variant form categories " << vcategory << '!' << endl;
            return false;
        }
        
        if (!pvterms.emplace(name, pvtermV).second)
        {
            cout << "Multiple definition of pvterm " << name << "!" << endl;
            return false;
        }
    }
    
    for (auto string = language.FirstChildElement("string"); string.ToElement();
        string = string.NextSiblingElement("string"))
    {
        LangString stringV;
        
        auto name = pickValidAttr(string, "String", "name");
        if (!name) return false;
        
        if (string.FirstChild().ToNode() != nullptr)
        {
            auto text = string.ToElement()->GetText();
            if (!text || string.FirstChild().ToNode() != string.LastChild().ToNode())
            {
                cout << "String " << name;
                cout << " text must have only an expression (and not any other tag)!" << endl;
                return false;
            }
            
            stringV.string = text;
        }
        else
        {
            cout << "String " << name;
            cout << " must have some text!" << endl;
            return false;
        }
        
        auto variant = string.ToElement()->Attribute("variant");
        if (variant)
        {
            for (auto c = variant; *c; c++)
            if (!((*c >= 'A' && *c <= 'Z') || 
                  (*c >= 'a' && *c <= 'z') || 
                  (*c >= '0' && *c <= '9') || 
                   *c == '-' || *c == ' ' || *c == ':'))
            {
                cout << "Variant in string " << name;
                cout << " must be composed of letters, digits, hyphens, commas or spaces!" << endl;
                return false;
            }

            auto cur = variant;
            while (*cur)
            {
                while (isSpace(*cur)) cur++; if (!*cur) break;
                auto beg = cur;
                while (*cur && !isSpace(*cur)) cur++;
                auto com = beg;
                while (com != cur && *com != ':') com++;
                if (*com != ':')
                {
                    cout << "Variant pair in string " << name << " must be in form category:type!";
                    return false;
                }
                
                std::string category(beg, com);
                std::string type(com+1, cur);
                if (!isValidName(com+1, cur))
                {
                    cout << "Variant pair in string " << name << " must be in form category:type!";
                    return false;
                }
                
                auto catIt = variantForms.find(category);
                if (catIt == variantForms.end())
                {
                    cout << "There is no variant form of category " << category;
                    cout << " (defined in variant pair in string " << name << ")!" << endl;
                    return false;
                }
                
                auto typeIt = catIt->second.variantTypes.find(type);
                if (typeIt == catIt->second.variantTypes.end())
                {
                    cout << "There is no variant form of type " << type << " of category " << category;
                    cout << " (defined in variant pair in string " << name << ")!" << endl;
                    return false;
                }
                
                stringV.vcategoryAssociations.emplace(distance(variantForms.begin(), catIt),
                    distance(catIt->second.variantTypes.begin(), typeIt));
            }
        }
        
        if (string.ToElement()->BoolAttribute("override"))
            strings.erase(name);
        
        if (!strings.emplace(name, stringV).second)
        {
            cout << "Multiple definition of string " << name << "!" << endl;
            return false;
        }
    }
    
    for (auto formatter = language.FirstChildElement("formatter"); formatter.ToElement();
        formatter = formatter.NextSiblingElement("formatter"))
    {
        Formatter formatterV;
        
        auto name = pickValidAttr(formatter, "Formatter", "name");
        if (!name) return false;
        
        if (formatter.FirstChild().ToNode() != nullptr)
        {
            auto text = formatter.ToElement()->GetText();
            if (!text || formatter.FirstChild().ToNode() != formatter.LastChild().ToNode())
            {
                cout << "Formatter " << name;
                cout << " text must have only an expression (and not any other tag)!" << endl;
                return false;
            }
            
            string textString(text);
            size_t p = 0;
            
            while (p != string::npos)
            {
                size_t c = textString.find('{', p), h1, h2;
                if (c == string::npos) break;
                p = textString.find('}', c);
                
                if (c == textString.size()-1 || p == string::npos)
                {
                    cout << "Unmatched character '{' on formatter " << name << "!";
                    return false;
                }
                
                Formatter::Specifier specifier;
                specifier.type = 255;
                specifier.byteLocation = c;
                switch (textString[c+1])
                {
                    case '$': case '#':
                        if (!isValidName(&textString[c+2], false, '}'))
                        {
                            cout << "Name inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        
                        specifier.type = textString[c+1] == '$' ? 0 : 1;
                        specifier.specifier1 = textString.substr(c+2, p-c-2);
                        break;
                        
                    case '@':
                        if (textString[c+2] == '%') goto pvTermCase;
                    
                        if (!isValidName(&textString[c+2], false, ':'))
                        {
                            cout << "Name inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        h1 = textString.find(':', c+2);
                        if (h1 > p)
                        {
                            cout << "Pterm specifier on formatter " << name;
                            cout << " must be of type {@pterm:name}!" << endl;
                            return false;
                        }
                        if (!isValidName(&textString[h1+1], false, '}'))
                        {
                            cout << "Pterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        
                        specifier.type = 2;
                        specifier.specifier1 = textString.substr(h1+1, p-h1-1);
                        
                        {
                            auto ptermIt = pterms.find(textString.substr(c+2, h1-c-2));
                            if (ptermIt == pterms.end())
                            {
                                cout << "There is no pterm " << textString.substr(c+2, h1-c-2);
                                cout << " (referenced in specifier in formatter " << name << ")!" << endl;
                            }
                            specifier.term = distance(pterms.begin(), ptermIt);
                        }
                        break;
                        
                    case '%':
                        if (textString[c+2] == '@') goto vpTermCase;
                    
                        if (!isValidName(&textString[c+2], false, ':'))
                        {
                            cout << "Name inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        h1 = textString.find(':', c+2);
                        if (h1 > p)
                        {
                            cout << "Vterm specifier on formatter " << name;
                            cout << " must be of type {%vterm:name}!" << endl;
                            return false;
                        }
                        if (!isValidName(&textString[h1+1], false, '}'))
                        {
                            cout << "Vterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        
                        specifier.type = 3;
                        specifier.specifier1 = textString.substr(h1+1, p-h1-1);
                        
                        {
                            auto vtermIt = vterms.find(textString.substr(c+2, h1-c-2));
                            if (vtermIt == vterms.end())
                            {
                                cout << "There is no vterm " << textString.substr(c+2, h1-c-2);
                                cout << " (referenced in specifier in formatter " << name << ")!" << endl;
                            }
                            specifier.term = distance(vterms.begin(), vtermIt);
                        }
                        break;
                        
                    pvTermCase:
                        if (!isValidName(&textString[c+3], false, ':'))
                        {
                            cout << "Name inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        h1 = textString.find(':', c+3);
                        if (h1 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {@%pvterm:number-p:string-v}!" << endl;
                            return false;
                        }
                        if (!isValidName(&textString[h1+1], false, ':'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        h2 = textString.find(':', h1+1);
                        if (h2 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {@%pvterm:number-p:string-v}!" << endl;
                            return false;
                        }
                        if (!isValidName(&textString[h2+1], false, '}'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        
                        specifier.type = 4;
                        specifier.specifier1 = textString.substr(h1+1, h2-h1-1);
                        specifier.specifier2 = textString.substr(h2+1, p-h2-1);
                        
                        {
                            auto pvtermIt = pvterms.find(textString.substr(c+3, h1-c-3));
                            if (pvtermIt == pvterms.end())
                            {
                                cout << "There is no pvterm " << textString.substr(c+3, h1-c-3);
                                cout << " (referenced in specifier in formatter " << name << ")!" << endl;
                            }
                            specifier.term = distance(pvterms.begin(), pvtermIt);
                        }
                        break;
                        
                    vpTermCase:
                        if (!isValidName(&textString[c+3], false, ':'))
                        {
                            cout << "Name inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        h1 = textString.find(':', c+3);
                        if (h1 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {%@pvterm:string-v:number-p}!" << endl;
                            return false;
                        }
                        if (!isValidName(&textString[h1+1], false, ':'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        h2 = textString.find(':', h1+1);
                        if (h2 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {%@pvterm:string-v:number-p}!" << endl;
                            return false;
                        }
                        if (!isValidName(&textString[h2+1], false, '}'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return false;
                        }
                        
                        specifier.type = 4;
                        specifier.specifier2 = textString.substr(h1+1, h2-h1-1);
                        specifier.specifier1 = textString.substr(h2+1, p-h2-1);
                        
                        {
                            auto pvtermIt = pvterms.find(textString.substr(c+3, h1-c-3));
                            if (pvtermIt == pvterms.end())
                            {
                                cout << "There is no pvterm " << textString.substr(c+3, h1-c-3);
                                cout << " (referenced in specifier in formatter " << name << ")!" << endl;
                            }
                            specifier.term = distance(pvterms.begin(), pvtermIt);
                        }
                        break;
                }
                
                if (specifier.type != 255)
                {
                    formatterV.specifiers.push_back(specifier);
                    textString.erase(c, p-c+1);
                    p = c;
                }
            }
            
            formatterV.modelString = textString;
        }
        else
        {
            cout << "Formatter " << name;
            cout << " must have some text!" << endl;
            return false;
        }
        
        if (formatter.ToElement()->BoolAttribute("override"))
            formatters.erase(name);
        
        if (!formatters.emplace(name, formatterV).second)
        {
            cout << "Multiple definition of formatter " << name << "!" << endl;
            return false;
        }
    }
    
    return true;
}

int exportLanguage(string inFile, string outFile)
{
    XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return false;
	}

	XMLHandle docHandle(doc);

	ofstream out(outFile, ios::out | ios::binary);
	out.write("LANG", 4);
    
    auto language = docHandle.FirstChildElement("language");
    
    auto name = language.ToElement()->Attribute("name");
    auto posixLocale = language.ToElement()->Attribute("posix-locale");
    auto windowsLocale = language.ToElement()->Attribute("windows-locale");
    auto fontName = language.ToElement()->Attribute("font-name");
    
    for (auto str : { name, posixLocale, windowsLocale, fontName })
    {
        if (!str)
        {
            write_varlength(out, 0);
            continue;
        }
        
        if (!isASCII(str))
        {
            cout << "Attributes on language tag must be ASCII strings!" << endl;
            return false;
        }
        
        write_str(out, str);
    }
    
    bool isRTL = language.ToElement()->BoolAttribute("rtl");
    float fontSizeFactor = language.ToElement()->FloatAttribute("font-size-factor", 1);
    
    if (fontSizeFactor <= 0)
    {
        cout << "Language's font size factor must be positive!" << endl;
        return false;
    }
    
    if (isRTL) fontSizeFactor = -fontSizeFactor;
    out.write((const char*)&fontSizeFactor, sizeof(float));
    
    map<string,PluralForm> pluralForms;
    map<string,VariantForm> variantForms;
    map<string,Pterm> pterms;
    map<string,Vterm> vterms;
    map<string,Pvterm> pvterms;
    map<string,LangString> strings;
    map<string,Formatter> formatters;
    
    if (!parseLanguage(doc, inFile, pluralForms, variantForms, pterms, vterms, pvterms, strings, formatters))
    {
        cout << "Error while parsing file " << inFile << "." << endl;
        return -1;
    }
        
    write_varlength(out, strings.size());
    for (const auto& string : strings)
    {
        write_str(out, string.first);
        write_str(out, string.second.string);
        write_varlength(out, string.second.vcategoryAssociations.size());
        for (const auto& pair : string.second.vcategoryAssociations)
        {
            write_varlength(out, pair.first);
            write_varlength(out, pair.second);
        }
    }
    
    write_varlength(out, formatters.size());
    for (const auto& formatter : formatters)
    {
        write_str(out, formatter.first);
        write_str(out, formatter.second.modelString);
        write_varlength(out, formatter.second.specifiers.size());
        
        for (const auto& specifier : formatter.second.specifiers)
        {
            write_varlength(out, specifier.byteLocation);
            out.write((const char*)&specifier.type, sizeof(uint8_t));
            
            if (specifier.type >= 2) write_varlength(out, specifier.term);
            
            write_str(out, specifier.specifier1);
            if (specifier.type == 4)
                write_str(out, specifier.specifier2);
        }
    }
    
    write_varlength(out, pluralForms.size());
    for (const auto& pluralForm : pluralForms)
    {
        write_varlength(out, pluralForm.second.pluralUnits.size());
        for (const auto& p : pluralForm.second.pluralUnits)
        {
            if (p.second) writeExpressionTree(out, *p.second);
            else
            {
                write_varlength(out, 1);
                write_varlength(out, 16);
            }
        }
    }
    
    write_varlength(out, pterms.size());
    for (const auto& pterm : pterms)
    {
        auto pluralFormIt = pluralForms.find(pterm.second.category);
        write_varlength(out, distance(pluralForms.begin(), pluralFormIt));
        
        for (size_t i = 0; i < pluralFormIt->second.pluralUnits.size(); i++)
            write_str(out, pterm.second.variants.find(pluralFormIt->second.pluralUnits.at(i).first)->second);
    }
    
    write_varlength(out, vterms.size());
    for (const auto& vterm : vterms)
    {
        write_varlength(out, vterm.second.categories.size());
        
        vector<map<string,VariantForm>::const_iterator> catIterators;
        for (const auto& v : vterm.second.categories)
        {
            auto catIt = variantForms.find(v);
            write_varlength(out, distance(variantForms.begin(), catIt));
            catIterators.push_back(catIt);
        }
        
        write_varlength(out, vterm.second.variants.size());
        for (const auto& v : vterm.second.variants)
        {
            for (size_t i = 0; i < v.types.size(); i++)
            {
                auto typeIt = catIterators[i]->second.variantTypes.find(v.types[i]);
                write_varlength(out, distance(catIterators[i]->second.variantTypes.begin(), typeIt));
            }
            
            write_str(out, v.string);
        }
    }
    
    write_varlength(out, pvterms.size());
    for (const auto& pvterm : pvterms)
    {
        auto pluralFormIt = pluralForms.find(pvterm.second.pcategory);
        write_varlength(out, distance(pluralForms.begin(), pluralFormIt));
        write_varlength(out, pvterm.second.vcategories.size());
        
        vector<map<string,VariantForm>::const_iterator> catIterators;
        for (const auto& v : pvterm.second.vcategories)
        {
            auto catIt = variantForms.find(v);
            write_varlength(out, distance(variantForms.begin(), catIt));
            catIterators.push_back(catIt);
        }
        
        write_varlength(out, pvterm.second.variants.size());
        for (const auto& v : pvterm.second.variants)
        {
            auto ptypeIt = find_if(pluralFormIt->second.pluralUnits.begin(),
                pluralFormIt->second.pluralUnits.end(),
                [&] (const auto& p) { return p.first == v.ptype; });
            write_varlength(out, distance(pluralFormIt->second.pluralUnits.begin(), ptypeIt));
            
            for (size_t i = 0; i < v.vtypes.size(); i++)
            {
                auto typeIt = catIterators[i]->second.variantTypes.find(v.vtypes[i]);
                write_varlength(out, distance(catIterators[i]->second.variantTypes.begin(), typeIt));
            }
            
            write_str(out, v.string);
        }
    }
    
    return 0;
}
