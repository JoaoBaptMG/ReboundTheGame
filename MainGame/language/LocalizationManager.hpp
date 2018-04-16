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

#include <vector>
#include <string>
#include <set>
#include <list>
#include <unordered_map>
#include <functional>
#include <ContainerEntry.hpp>
#include "LangID.hpp"
#include "LanguageDescriptor.hpp"
#include "execDir.hpp"

using StringSpecifierMap = std::unordered_map<LangID,LangID>;
using RawSpecifierMap = std::unordered_map<LangID,std::string>;
using NumberSpecifierMap = std::unordered_map<LangID,size_t>;

class LocalizationManager final
{
public:
    using Callback = std::function<void()>;
    using CallbackList = std::list<Callback>;
    using CallbackEntry = util::ContainerEntry<CallbackList>;

private:
    bool descriptorDebug, error;
    LanguageDescriptor languageDescriptor;
    CallbackList languageChangeCallbacks;
    
public:
    LocalizationManager(bool debug = false) : descriptorDebug(debug), error(true) {}
    ~LocalizationManager() {}
    
    void loadLanguageDescriptor(std::string name);
    
    std::string getString(const LangID& id) const;
    std::string getFormattedString(const LangID& id, const StringSpecifierMap &stringSpecifiers,
        const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers) const;
        
    std::string getFontName() const;
        
    float getFontSizeFactor() const;
    bool isRTL() const;

    CallbackEntry registerLanguageChangeCallback(Callback callback);
    
private:
    std::string buildFormat(const LangID& id, const StringSpecifierMap &stringSpecifiers,
        const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers) const;
};

std::set<std::string> getAllLanguageDescriptors();
std::string getLanguageDescriptorName(std::string lang);
std::string languageDescriptorForLocale(std::string locale);
std::string getPathOfLanguageDescriptor(std::string name);

