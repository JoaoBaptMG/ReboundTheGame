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

int exportLanguage(string inFile, string outFile)
{
    XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return -1;
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
            return -1;
        }
        
        write_str(out, str);
    }
    
    bool isRTL = language.ToElement()->BoolAttribute("rtl");
    float fontSizeFactor = language.ToElement()->FloatAttribute("font-size-factor", 1);
    
    if (fontSizeFactor <= 0)
    {
        cout << "Language's font size factor must be positive!" << endl;
        return -1;
    }
    
    if (isRTL) fontSizeFactor = -fontSizeFactor;
    out.write((const char*)&fontSizeFactor, sizeof(float));
    
    map<string,PluralForm> pluralForms;
    for (auto pform = language.FirstChildElement("plural-forms"); pform.ToElement();
        pform = pform.NextSiblingElement("plural-forms"))
    {
        auto category = pform.ToElement()->Attribute("category");
        if (!category) category = "default";
        if (!isValidName(category))
        {
            cout << "Plural form category must consist of letters, digits or hyphens!" << endl;
            return -1;
        }
        
        PluralForm pluralForm;
        for (auto pf = pform.FirstChildElement("plural-form"); pf.ToElement();
            pf = pf.NextSiblingElement("plural-form"))
        {
            auto type = pickValidAttrs(pf, "Plural form", "type", std::string(" defined on category ") + category);
            if (!type) return -1;
            
            for (const auto& p : pluralForm.pluralUnits)
            {
                if (p.first == type)
                {
                    cout << "Multiple definition of plural form of type " << type;
                    cout << " defined on category " << category << '!' << endl;
                    return -1;
                }
            }
            
            if (pf.FirstChild().ToNode() != nullptr)
            {
                auto text = pf.ToElement()->GetText();
                if (!text || pf.FirstChild().ToNode() != pf.LastChild().ToNode())
                {
                    cout << "Plural form of type " << type << " defined on category " << category;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return -1;
                }
                
                try
                {
                    pluralForm.pluralUnits.emplace_back(type, compileExpression(text));
                }
                catch (const runtime_error &err)
                {
                    cout << "Invalid expression on plural form of type " << type << " on category " << category;
                    cout << ": " << err.what() << endl;
                    return -1;
                }
            }
            else pluralForm.pluralUnits.emplace_back(type, nullptr);
        }
        
        if (!pluralForms.emplace(category, move(pluralForm)).second)
        {
            cout << "Multiple definition of plural form of category " << category << '!' << endl;
            return -1;
        }
    }
    
    map<string,VariantForm> variantForms;
    for (auto vform = language.FirstChildElement("variant-forms"); vform.ToElement();
        vform = vform.NextSiblingElement("variant-forms"))
    {
        auto category = pickValidAttr(vform, "Variant form", "category");
        if (!category) return -1;
        
        VariantForm variantForm;
        for (auto vf = vform.FirstChildElement("variant-form"); vf.ToElement();
            vf = vf.NextSiblingElement("variant-form"))
        {
            auto type = pickValidAttrs(vf, "Variant form", "type", std::string(" defined on category ") + category);
            if (!type) return -1;
            
            if (vf.FirstChild().ToNode() != nullptr)
            {
                cout << "Variant form of type " << type << " on category " << category << " must have no children!";
                return -1;
            }
            
            if (!variantForm.variantTypes.emplace(type).second)
            {
                cout << "Multiple definition of variant form of type " << type;
                cout << " defined on category " << category << '!' << endl;
                return -1;
            }
        }
        
        if (!variantForms.emplace(category, variantForm).second)
        {
            cout << "Multiple definition of variant form of category " << category << '!' << endl;
            return -1;
        }
    }
    
    map<string,Pterm> pterms;
    for (auto pterm = language.FirstChildElement("pterm"); pterm.ToElement();
        pterm = pterm.NextSiblingElement("pterm"))
    {
        auto name = pickValidAttr(pterm, "Pterm", "name");
        if (!name) return -1;

        auto category = pterm.ToElement()->Attribute("category");
        if (!category) category = "default";
        if (!isValidName(category))
        {
            cout << "Pterm category must consist of letters, digits or hyphens!" << endl;
            return -1;
        }
        
        auto pluralFormIt = pluralForms.find(category);
        if (pluralFormIt == pluralForms.end())
        {
            cout << "There is no plural form of category " << category;
            cout << " (of pterm of name " << name << ")!" << endl;
            return -1;
        }
        
        Pterm ptermV;
        ptermV.category = category;
        
        for (auto variant = pterm.FirstChildElement("variant"); variant.ToElement();
            variant = variant.NextSiblingElement("variant"))
        {
            auto type = pickValidAttrs(variant, "Variant", "type", std::string(" defined on pterm ") + name);
            if (!type) return -1;
            
            auto typeIt = find_if(pluralFormIt->second.pluralUnits.begin(), pluralFormIt->second.pluralUnits.end(),
                [=] (const auto& p) { return p.first == type; });
            if (typeIt == pluralFormIt->second.pluralUnits.end())
            {
                cout << "There is no plural form of type " << type << " of category " << category;
                cout << " (of pterm variant of name " << name << ")!" << endl;
                return -1;
            }
            
            if (variant.FirstChild().ToNode() != nullptr)
            {
                auto text = variant.ToElement()->GetText();
                if (!text || variant.FirstChild().ToNode() != variant.LastChild().ToNode())
                {
                    cout << "Variant of type " << type << " defined on pterm " << name;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return -1;
                }
                
                if (!ptermV.variants.emplace(type, text).second)
                {
                    cout << "Multiple definition of variant type " << type << " on pterm " << name << '!' << endl;
                    return -1;
                }
            }
            else
            {
                cout << "Variant of type " << type << " defined on pterm " << name;
                cout << " must have some text!" << endl;
                return -1;
            }
        }
        
        if (pluralFormIt->second.pluralUnits.size() != ptermV.variants.size())
        {
            cout << "Pterm " << name << " must define all types of plural form category " << category << '!' << endl;
            return -1;
        }
        
        if (!pterms.emplace(name, ptermV).second)
        {
            cout << "Multiple definition of pterm " << name << "!" << endl;
            return -1;
        }
    }
    
    map<string,Vterm> vterms;
    for (auto vterm = language.FirstChildElement("vterm"); vterm.ToElement();
        vterm = vterm.NextSiblingElement("vterm"))
    {
        auto name = pickValidAttr(vterm, "Vterm", "name");
        if (!name) return -1;
        
        auto category = pickValidAttr(vterm, "Vterm", "category", true);
        if (!category) return -1;
        
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
                return -1;
            }
            catIterators.push_back(catIt);
            vtermV.categories.push_back(catName);
        }
        
        if (catIterators.empty())
        {
            cout << "Vterm must have a category!" << endl;
            return -1;
        }
        
        for (auto variant = vterm.FirstChildElement("variant"); variant.ToElement();
            variant = variant.NextSiblingElement("variant"))
        {
            Vterm::Variant var;
            
            auto type = pickValidAttrs(variant, "Variant", "type", std::string(" defined on vterm ") + name, true);
            if (!type) return -1;
            
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
                    return -1;
                }
                
                var.types.push_back(typeName);
            }
            
            if (var.types.size() != catIterators.size())
            {
                cout << "Variant defined on vterm " << name;
                cout << " must provide variant types for all associated categories!" << endl;
                return -1;
            }
            
            for (const auto& other : vtermV.variants)
            {
                if (var.types == other.types)
                {
                    cout << "Multiple definition of variant type set " << type << " on vterm " << name << '!' << endl;
                    return -1;
                }
            }
            
            if (variant.FirstChild().ToNode() != nullptr)
            {
                auto text = variant.ToElement()->GetText();
                if (!text || variant.FirstChild().ToNode() != variant.LastChild().ToNode())
                {
                    cout << "Variant of type " << type << " defined on vterm " << name;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return -1;
                }
                
                var.string = text;
                vtermV.variants.push_back(move(var));
            }
            else
            {
                cout << "Variant of type " << type << " defined on vterm " << name;
                cout << " must have some text!" << endl;
                return -1;
            }
        }
        
        size_t variantTotals = 1;
        for (const auto& catIt : catIterators)
            variantTotals *= catIt->second.variantTypes.size();
        
        if (vtermV.variants.size() != variantTotals)
        {
            cout << "Vterm " << name << " must define all types of variant form categories " << category << '!' << endl;
            return -1;
        }
        
        if (!vterms.emplace(name, vtermV).second)
        {
            cout << "Multiple definition of vterm " << name << "!" << endl;
            return -1;
        }
    }
    
    map<string,Pvterm> pvterms;
    for (auto pvterm = language.FirstChildElement("pvterm"); pvterm.ToElement();
        pvterm = pvterm.NextSiblingElement("pvterm"))
    {
        auto name = pickValidAttr(pvterm, "Pvterm", "name");
        if (!name) return -1;
        
        auto pcategory = pvterm.ToElement()->Attribute("pcategory");
        if (!pcategory) pcategory = "default";
        if (!isValidName(pcategory))
        {
            cout << "Pterm pcategory must consist of letters, digits or hyphens!" << endl;
            return -1;
        }
        
        auto pluralFormIt = pluralForms.find(pcategory);
        if (pluralFormIt == pluralForms.end())
        {
            cout << "There is no plural form of category " << pcategory;
            cout << " (of pterm of name " << name << ")!" << endl;
            return -1;
        }
        
        auto vcategory = pickValidAttr(pvterm, "Pvterm", "vcategory", true);
        if (!vcategory) return -1;
        
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
                return -1;
            }
            catIterators.push_back(catIt);
            pvtermV.vcategories.push_back(catName);
        }
        
        if (catIterators.empty())
        {
            cout << "Pvterm must have a vcategory!" << endl;
            return -1;
        }

        for (auto variant = pvterm.FirstChildElement("variant"); variant.ToElement();
            variant = variant.NextSiblingElement("variant"))
        {
            Pvterm::Variant var;
            
            auto type = pickValidAttrs(variant, "Variant", "type", std::string(" defined on pvterm ") + name, true);
            if (!type) return -1;
            
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
                    return -1;
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
                    return -1;
                }
                
                var.vtypes.push_back(typeName);
            }
            
            if (var.vtypes.size() != catIterators.size())
            {
                cout << "Variant defined on pvterm " << name;
                cout << " must provide variant types for all associated vcategories!" << endl;
                return -1;
            }
            
            for (const auto& other : pvtermV.variants)
            {
                if (var.ptype == other.ptype && var.vtypes == other.vtypes)
                {
                    cout << "Multiple definition of variant type set " << type << " on pvterm " << name << '!' << endl;
                    return -1;
                }
            }
            
            if (variant.FirstChild().ToNode() != nullptr)
            {
                auto text = variant.ToElement()->GetText();
                if (!text || variant.FirstChild().ToNode() != variant.LastChild().ToNode())
                {
                    cout << "Variant of type " << type << " defined on pvterm " << name;
                    cout << " must have only an expression (and not any other tag)!" << endl;
                    return -1;
                }
                
                var.string = text;
                pvtermV.variants.push_back(move(var));
            }
            else
            {
                cout << "Variant of type " << type << " defined on pvterm " << name;
                cout << " must have some text!" << endl;
                return -1;
            }
        }
        
        size_t variantTotals = pluralFormIt->second.pluralUnits.size();
        for (const auto& catIt : catIterators)
            variantTotals *= catIt->second.variantTypes.size();
        
        if (pvtermV.variants.size() != variantTotals)
        {
            cout << "Pvterm " << name << " must define all types of plural form category " << pcategory;
            cout << " and variant form categories " << vcategory << '!' << endl;
            return -1;
        }
        
        if (!pvterms.emplace(name, pvtermV).second)
        {
            cout << "Multiple definition of pvterm " << name << "!" << endl;
            return -1;
        }
    }
    
    size_t strings = 0;
    for (auto string = language.FirstChildElement("string"); string.ToElement();
        string = string.NextSiblingElement("string"))
        strings++;
        
    write_varlength(out, strings);
    
    for (auto string = language.FirstChildElement("string"); string.ToElement();
        string = string.NextSiblingElement("string"))
    {
        auto name = pickValidAttr(string, "String", "name");
        if (!name) return -1;
        
        write_str(out, name);
        
        if (string.FirstChild().ToNode() != nullptr)
        {
            auto text = string.ToElement()->GetText();
            if (!text || string.FirstChild().ToNode() != string.LastChild().ToNode())
            {
                cout << "String " << name;
                cout << " text must have only an expression (and not any other tag)!" << endl;
                return -1;
            }
            
            write_str(out, text);
        }
        else
        {
            cout << "String " << name;
            cout << " must have some text!" << endl;
            return -1;
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
                return -1;
            }
            
            stringstream variantOut;
            size_t numVariants = 0;
            
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
                    return -1;
                }
                
                std::string category(beg, com);
                std::string type(com+1, cur);
                if (!isValidName(com+1, cur))
                {
                    cout << "Variant pair in string " << name << " must be in form category:type!";
                    return -1;
                }
                
                auto catIt = variantForms.find(category);
                if (catIt == variantForms.end())
                {
                    cout << "There is no variant form of category " << category;
                    cout << " (defined in variant pair in string " << name << ")!" << endl;
                    return -1;
                }
                
                auto typeIt = catIt->second.variantTypes.find(type);
                if (typeIt == catIt->second.variantTypes.end())
                {
                    cout << "There is no variant form of type " << type << " of category " << category;
                    cout << " (defined in variant pair in string " << name << ")!" << endl;
                    return -1;
                }
                
                numVariants++;
                write_varlength(variantOut, distance(variantForms.begin(), catIt));
                write_varlength(variantOut, distance(catIt->second.variantTypes.begin(), typeIt));
            }
            
            write_varlength(out, numVariants);
            out << variantOut.str();
        }
        else write_varlength(out, 0);
    }
    
    size_t numFormatters = 0;
    for (auto formatter = language.FirstChildElement("formatter"); formatter.ToElement();
        formatter = formatter.NextSiblingElement("formatter"))
        numFormatters++;
        
    write_varlength(out, numFormatters);
    
    for (auto formatter = language.FirstChildElement("formatter"); formatter.ToElement();
        formatter = formatter.NextSiblingElement("formatter"))
    {
        auto name = pickValidAttr(formatter, "Formatter", "name");
        if (!name) return -1;
        
        write_str(out, name);
        
        if (formatter.FirstChild().ToNode() != nullptr)
        {
            auto text = formatter.ToElement()->GetText();
            if (!text || formatter.FirstChild().ToNode() != formatter.LastChild().ToNode())
            {
                cout << "Formatter " << name;
                cout << " text must have only an expression (and not any other tag)!" << endl;
                return -1;
            }
            
            string textString(text);
            size_t p = 0;
        
            struct Specifier
            {
                size_t byteLocation;
                uint8_t type;
                std::string specifier1, specifier2;
                size_t term;
            };

            vector<Specifier> specifiers;
            
            while (p != string::npos)
            {
                size_t c = textString.find('{', p), h1, h2;
                if (c == string::npos) break;
                p = textString.find('}', c);
                
                if (c == textString.size()-1 || p == string::npos)
                {
                    cout << "Unmatched character '{' on formatter " << name << "!";
                    return -1;
                }
                
                Specifier specifier;
                specifier.type = 255;
                specifier.byteLocation = c;
                switch (textString[c+1])
                {
                    case '$': case '#':
                        if (!isValidName(&textString[c+2], false, '}'))
                        {
                            cout << "Name inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
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
                            return -1;
                        }
                        h1 = textString.find(':', c+2);
                        if (h1 > p)
                        {
                            cout << "Pterm specifier on formatter " << name;
                            cout << " must be of type {@pterm:name}!" << endl;
                            return -1;
                        }
                        if (!isValidName(&textString[h1+1], false, '}'))
                        {
                            cout << "Pterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
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
                            return -1;
                        }
                        h1 = textString.find(':', c+2);
                        if (h1 > p)
                        {
                            cout << "Vterm specifier on formatter " << name;
                            cout << " must be of type {%vterm:name}!" << endl;
                            return -1;
                        }
                        if (!isValidName(&textString[h1+1], false, '}'))
                        {
                            cout << "Vterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
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
                            return -1;
                        }
                        h1 = textString.find(':', c+3);
                        if (h1 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {@%pvterm:number-p:string-v}!" << endl;
                            return -1;
                        }
                        if (!isValidName(&textString[h1+1], false, ':'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
                        }
                        h2 = textString.find(':', h1+1);
                        if (h2 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {@%pvterm:number-p:string-v}!" << endl;
                            return -1;
                        }
                        if (!isValidName(&textString[h2+1], false, '}'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
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
                            return -1;
                        }
                        h1 = textString.find(':', c+3);
                        if (h1 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {%@pvterm:string-v:number-p}!" << endl;
                            return -1;
                        }
                        if (!isValidName(&textString[h1+1], false, ':'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
                        }
                        h2 = textString.find(':', h1+1);
                        if (h2 > p)
                        {
                            cout << "Pvterm specifier on formatter " << name;
                            cout << " must be of type {%@pvterm:string-v:number-p}!" << endl;
                            return -1;
                        }
                        if (!isValidName(&textString[h2+1], false, '}'))
                        {
                            cout << "Pvterm inside specifier on formatter " << name;
                            cout << " must consist of letters, digits or hyphens!" << endl;
                            return -1;
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
                    specifiers.push_back(specifier);
                    textString.erase(c, p-c+1);
                    p = c;
                }
            }
            
            write_str(out, textString);
            write_varlength(out, specifiers.size());
            for (const auto& specifier : specifiers)
            {
                write_varlength(out, specifier.byteLocation);
                out.write((const char*)&specifier.type, sizeof(uint8_t));
                
                if (specifier.type >= 2) write_varlength(out, specifier.term);
                
                write_str(out, specifier.specifier1);
                if (specifier.type == 4)
                    write_str(out, specifier.specifier2);
            }
        }
        else
        {
            cout << "Formatter " << name;
            cout << " must have some text!" << endl;
            return -1;
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
    
    //write_varlength(out, variantForms.size());
    //for (const auto& variantForm : variantForms)
    //    write_varlength(out, variantForm.second.variantTypes.size());
    
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
