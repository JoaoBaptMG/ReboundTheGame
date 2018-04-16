//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//


#include "LanguageDescriptor.hpp"

#include <algorithm>
#include <utility>
#include <streamReaders.hpp>

bool readFromStream(sf::InputStream& stream, LangString& string)
{
    if (!readFromStream(stream, string.string)) return false;
    
    size_t size;
    if (!readFromStream(stream, varLength(size))) return false;
    
    std::vector<std::pair<size_t,size_t>> pairs(size);
    for (auto& pair : pairs)
        if (!readFromStream(stream, varLength(pair.first), varLength(pair.second))) return false;
    
    string.vcategoryAssociations.insert(pairs.begin(), pairs.end());
    return true;
}

bool readFromStream(sf::InputStream& stream, Formatter& formatter)
{
    size_t size;
    if (!readFromStream(stream, formatter.modelString, varLength(size))) return false;
    
    formatter.specifiers.resize(size);
    
    return std::all_of(formatter.specifiers.begin(), formatter.specifiers.end(),
    [&] (auto& specifier)
    {
        if (!readFromStream(stream, varLength(specifier.byteLocation), specifier.type))
            return false;
            
        switch (specifier.type)
        {
            case Formatter::Specifier::Type::Pterm:
            case Formatter::Specifier::Type::Vterm:
            case Formatter::Specifier::Type::Pvterm:
                if (!readFromStream(stream, varLength(specifier.term))) return false;
                
            default: break;
        }
        
        return readFromStream(stream, specifier.specifier1) &&
            (specifier.type != Formatter::Specifier::Type::Pvterm ||
                readFromStream(stream, specifier.specifier2));
    });
}

bool readFromStream(sf::InputStream& stream, PluralForm& pluralForm)
{
    return readFromStream(stream, pluralForm.pluralUnits);
}

bool readPterms(sf::InputStream& stream, std::vector<Pterm>& pterms, const std::vector<PluralForm>& pluralForms)
{
    size_t size;
    if (!readFromStream(stream, varLength(size))) return false;
    
    pterms.resize(size);
    
    for (auto& pterm : pterms)
    {
        if (!readFromStream(stream, varLength(pterm.category))) return false;
        
        pterm.variants.resize(pluralForms[pterm.category].pluralUnits.size());
        for (auto& variant : pterm.variants)
            if (!readFromStream(stream, variant)) return false;
    }
        
    return true;
}

static bool readVarLengthVector(sf::InputStream& stream, std::vector<size_t>& vec)
{
    size_t size;
    if (!readFromStream(stream, varLength(size))) return false;
    
    vec.resize(size);
    for (auto& v : vec) if (!readFromStream(stream, varLength(v))) return false;
    return true;
}

bool readFromStream(sf::InputStream& stream, Vterm& vterm)
{
    size_t size;
    if (!(readVarLengthVector(stream, vterm.categories) &&
        readFromStream(stream, varLength(size)))) return false;
    
    vterm.variants.resize(size);
    for (auto& variant : vterm.variants)
    { 
        variant.types.resize(vterm.categories.size());
        for (auto& v : variant.types) if (!readFromStream(stream, varLength(v))) return false;
        if (!readFromStream(stream, variant.string)) return false;
    }
    
    return true;
}

bool readFromStream(sf::InputStream& stream, Pvterm& pvterm)
{
    size_t size;
    if (!(readFromStream(stream, varLength(pvterm.pcategory)) && readVarLengthVector(stream, pvterm.vcategories) &&
        readFromStream(stream, varLength(size)))) return false;
    
    pvterm.variants.resize(size);
    for (auto& variant : pvterm.variants)
    { 
        if (!readFromStream(stream, varLength(variant.ptype))) return false;
        variant.vtypes.resize(pvterm.vcategories.size());
        for (auto& v : variant.vtypes) if (!readFromStream(stream, varLength(v))) return false;
        if (!readFromStream(stream, variant.string)) return false;
    }
    
    return true;
}

bool readFromStream(sf::InputStream& stream, LanguageDescriptor& descriptor)
{
    return checkMagic(stream, "LANG") &&
        readFromStream(stream, descriptor.name, descriptor.posixLocale, descriptor.windowsLocale,
            descriptor.fontName, descriptor.fontSizeFactorRTL, descriptor.strings,
            descriptor.formatters, descriptor.pluralForms) &&
            readPterms(stream, descriptor.pterms, descriptor.pluralForms) &&
            readFromStream(stream, descriptor.vterms, descriptor.pvterms);
}

size_t PluralForm::pick(size_t x) const
{
    size_t i;
    for (i = 0; i < pluralUnits.size(); i++)
    {
        if (runExpression(pluralUnits.at(i), x))
            break;
    }
    
    return i;
}

std::vector<size_t> LangString::typesForVariantCategories(const std::vector<size_t>& categories) const
{
    std::vector<size_t> types;
    
    for (size_t category : categories)
    {
        auto it = vcategoryAssociations.find(category);
        if (it == vcategoryAssociations.end())
            return std::vector<size_t>{};
        types.push_back(it->second);
    }
    
    return types;
}
