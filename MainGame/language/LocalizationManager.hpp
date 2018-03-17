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

