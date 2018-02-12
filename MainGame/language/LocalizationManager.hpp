#pragma once

#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include "LangID.hpp"
#include "LanguageDescriptor.hpp"
#include "execDir.hpp"

using StringSpecifierMap = std::unordered_map<LangID,LangID>;
using NumberSpecifierMap = std::unordered_map<LangID,size_t>;

class LocalizationManager final
{
    bool descriptorDebug, error;
    LanguageDescriptor languageDescriptor;
    
public:
    LocalizationManager(bool debug = false) : descriptorDebug(debug), error(true) {}
    ~LocalizationManager() {}
    
    void loadLanguageDescriptor(std::string name);
    
    std::string getString(const LangID& id) const;
    std::string getFormattedString(const LangID& id, const StringSpecifierMap &stringSpecifiers,
        const NumberSpecifierMap& numberSpecifiers) const;
        
    std::string getFontName() const;
        
    float getFontSizeFactor() const;
    bool isRTL() const;
    
private:
    std::string buildFormat(const LangID& id, const StringSpecifierMap &stringSpecifiers,
        const NumberSpecifierMap& numberSpecifiers) const;
};

std::set<std::string> getAllLanguageDescriptors();
std::string languageDescriptorForLocale(std::string locale);
std::string getPathOfLanguageDescriptor(std::string name);

