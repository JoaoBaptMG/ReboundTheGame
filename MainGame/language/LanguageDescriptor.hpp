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


#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cmath>


#include "LangID.hpp"
#include "data/ExpressionCommands.hpp"

struct LangString final
{
    std::string string;
    std::unordered_map<size_t,size_t> vcategoryAssociations;
    
    std::vector<size_t> typesForVariantCategories(const std::vector<size_t>& categories) const;
};

struct Formatter final
{
    struct Specifier
    {
        size_t byteLocation;
        enum class Type : uint8_t { String, Number, Pterm, Vterm, Pvterm } type;
        LangID specifier1, specifier2;
        size_t term;
    };
    
    std::string modelString;
    std::vector<Specifier> specifiers;
};

struct PluralForm final
{
    std::vector<ExpressionCommands> pluralUnits;
    size_t pick(size_t x) const;
};

struct Pterm final
{
    size_t category;
    std::vector<std::string> variants;
};

struct Vterm final
{
    struct Variant
    {
        std::vector<size_t> types;
        std::string string;
    };
    
    std::vector<size_t> categories;
    std::vector<Variant> variants;
};

struct Pvterm final
{
    struct Variant
    {
        size_t ptype;
        std::vector<size_t> vtypes;
        std::string string;
    };
    
    size_t pcategory;
    std::vector<size_t> vcategories;
    std::vector<Variant> variants;
};

struct LanguageDescriptor final
{
    std::string name, posixLocale, windowsLocale, fontName;
    float fontSizeFactorRTL;
    
    std::unordered_map<LangID,LangString> strings;
    std::unordered_map<LangID,Formatter> formatters;
    
    std::vector<PluralForm> pluralForms;
    std::vector<Pterm> pterms;
    std::vector<Vterm> vterms;
    std::vector<Pvterm> pvterms;
    
    float getFontSizeFactor() const { return fabsf(fontSizeFactorRTL); }
    bool isRTL() const { return fontSizeFactorRTL < 0; }
};

bool readFromStream(InputStream& stream, LangString& string);
bool readFromStream(InputStream& stream, Formatter& formatter);
bool readFromStream(InputStream& stream, PluralForm& pluralForm);
bool readPterms(InputStream& stream, std::vector<Pterm>& pterms, const std::vector<PluralForm>& pluralForms);
bool readFromStream(InputStream& stream, Vterm& vterm);
bool readFromStream(InputStream& stream, Pvterm& pvterm);
bool readFromStream(InputStream& stream, LanguageDescriptor& descriptor);
