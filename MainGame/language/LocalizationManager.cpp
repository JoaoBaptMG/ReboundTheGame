#include "LocalizationManager.hpp"
#include <SFML/System.hpp>

constexpr auto DefaultLanguageDescriptor = "en-us.lang";
constexpr auto DefaultFontName = "mplus-1m-medium.ttf";

auto buildErrorForString(const LangID& id)
{
    return "!!! STRING " + id + " NOT FOUND IN DESCRIPTOR !!!";
}

auto buildErrorForFormatter(const LangID& id)
{
    return "!!! FORMATTER " + id + " NOT FOUND IN DESCRIPTOR !!!";
}

auto buildErrorForStringSpecifier(const LangID& id)
{
    return "!!! STRING SPECIFIER " + id + " NOT PASSED AS PARAMETER !!!";
}

auto buildErrorForNumberSpecifier(const LangID& id)
{
    return "!!! NUMBER SPECIFIER " + id + " NOT PASSED AS PARAMETER !!!";
}

auto buildErrorForNumberPterm(size_t x)
{
    return "!!! NUMBER " + std::to_string(x) + " FAILED ALL PLURAL FORMS !!!";
}

auto buildErrorForStringVariant(const LangID& id)
{
    return "!!! STRING " + id + " DON'T HAVE ALL NECESSARY VARIANTS !!!";
}

std::string getPathOfLanguageDescriptor(std::string name)
{
    auto pos = name.find_last_of("\\/");
    if (pos == std::string::npos)
        pos = decltype(pos)(0);
    else pos++;
    
    return getExecutableDirectory() + "/Languages/" + name.substr(pos);
}

std::set<std::string> getAllLanguageDescriptors()
{
    static std::set<std::string> descs;
    
    if (descs.empty())
    {
        auto vector = getAllFilesInDir(getExecutableDirectory() + "/Languages");
        
        for (const auto& str : vector)
            if (str.find(".lang") != std::string::npos)
                descs.insert(str);
    }
        
    return descs;
}

bool isLanguageDescriptorPresent(std::string name)
{
    auto descs = getAllLanguageDescriptors();
    return descs.find(name) != descs.end();
}

std::string getLanguageDescriptorName(std::string lang)
{
    LanguageDescriptor languageDescriptor;
    sf::FileInputStream file;

    if (!file.open(getPathOfLanguageDescriptor(lang)) ||
        !readFromStream(file, languageDescriptor))
        return "";

    auto it = languageDescriptor.strings.find("metadata-english-name");
    if (it == languageDescriptor.strings.end())
        return "Unknown Language";

    auto it2 = languageDescriptor.strings.find("metadata-native-name");
    if (it2 == languageDescriptor.strings.end())
        return it->second.string;

    return it2->second.string + " / " + it->second.string;
}

void LocalizationManager::loadLanguageDescriptor(std::string name)
{
    error = false;
    
    if (!isLanguageDescriptorPresent(name)) error = true;
    else
    {
        sf::FileInputStream file;
        if (!file.open(getPathOfLanguageDescriptor(name)) ||
            !readFromStream(file, languageDescriptor))
            error = true;
    }

    for (auto& callback : languageChangeCallbacks) callback();
}

std::string LocalizationManager::getString(const LangID& id) const
{
    if (error) return "";
    
    auto it = languageDescriptor.strings.find(id);
    if (it != languageDescriptor.strings.end())
        return it->second.string;
    
    return descriptorDebug ? buildErrorForString(id) : "";
}

std::string LocalizationManager::getFormattedString(const LangID& id, const StringSpecifierMap &stringSpecifiers,
    const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers) const
{
    if (error) return "";
    
    auto it = languageDescriptor.formatters.find(id);
    if (it != languageDescriptor.formatters.end())
        return buildFormat(id, stringSpecifiers, numberSpecifiers, rawSpecifiers);
    
    return descriptorDebug ? buildErrorForFormatter(id) : "";
}

float LocalizationManager::getFontSizeFactor() const
{
    if (error) return 1;
    return languageDescriptor.getFontSizeFactor();
}

bool LocalizationManager::isRTL() const
{
    if (error) return false;
    return languageDescriptor.isRTL();
}

std::string LocalizationManager::buildFormat(const LangID& id, const StringSpecifierMap &stringSpecifiers,
    const NumberSpecifierMap& numberSpecifiers, const RawSpecifierMap& rawSpecifiers) const
{
    const auto& formatter = languageDescriptor.formatters.at(id);
    
    size_t addedBytes = 0;
    std::string builtString = formatter.modelString;
    
    for (const auto& specifier : formatter.specifiers)
    {
        std::string replacement;
        
        switch (specifier.type)
        {
            case Formatter::Specifier::Type::String:
            {
                auto it = stringSpecifiers.find(specifier.specifier1);
                auto it2 = rawSpecifiers.find(specifier.specifier1);
                if (it == stringSpecifiers.end() && it2 == stringSpecifiers.end())
                {
                    replacement = descriptorDebug ? buildErrorForStringSpecifier(specifier.specifier1) : "";
                    break;
                }

                if (it2 == stringSpecifiers.end())
                    replacement = getString(it->second);
                else replacement = it2->second;
            } break;
            
            case Formatter::Specifier::Type::Number:
            {
                auto it = numberSpecifiers.find(specifier.specifier1);
                if (it == numberSpecifiers.end())
                {
                    replacement = descriptorDebug ? buildErrorForStringSpecifier(specifier.specifier1) : "";
                    break;
                }
                
                replacement = std::to_string(it->second);
            } break;
            
            case Formatter::Specifier::Type::Pterm:
            {
                auto it = numberSpecifiers.find(specifier.specifier1);
                if (it == numberSpecifiers.end())
                {
                    replacement = descriptorDebug ? buildErrorForStringSpecifier(specifier.specifier1) : "";
                    break;
                }
                
                size_t x = it->second;
                const auto& pterm = languageDescriptor.pterms.at(specifier.term);
                const auto& pluralForm = languageDescriptor.pluralForms.at(pterm.category);

                size_t i = pluralForm.pick(x);
                if (i == pluralForm.pluralUnits.size())
                {
                    replacement = descriptorDebug ? buildErrorForNumberPterm(x) : "";
                    break;
                }
                
                replacement = pterm.variants.at(i);
            } break;
            
            case Formatter::Specifier::Type::Vterm:
            {
                auto it = stringSpecifiers.find(specifier.specifier1);
                if (it == stringSpecifiers.end())
                {
                    replacement = descriptorDebug ? buildErrorForStringSpecifier(specifier.specifier1) : "";
                    break;
                }
                
                auto id = it->second;

                auto it2 = languageDescriptor.strings.find(id);
                if (it2 == languageDescriptor.strings.end())
                {
                    replacement = descriptorDebug ? buildErrorForString(id) : "";
                    break;
                }
                
                const auto& vterm = languageDescriptor.vterms.at(specifier.term);
                auto types = it2->second.typesForVariantCategories(vterm.categories);
                
                if (types.empty())
                {
                    replacement = descriptorDebug ? buildErrorForStringVariant(id) : "";
                    break;
                }
                
                auto it3 = std::find_if(vterm.variants.begin(), vterm.variants.end(),
                [&] (const auto& variant) { return variant.types == types; });
                
                replacement = it3->string;
            } break;
            
            case Formatter::Specifier::Type::Pvterm:
            {
                auto it = numberSpecifiers.find(specifier.specifier1);
                if (it == numberSpecifiers.end())
                {
                    replacement = descriptorDebug ? buildErrorForStringSpecifier(specifier.specifier1) : "";
                    break;
                }
                
                size_t x = it->second;
                
                auto it2 = stringSpecifiers.find(specifier.specifier2);
                if (it2 == stringSpecifiers.end())
                {
                    replacement = descriptorDebug ? buildErrorForStringSpecifier(specifier.specifier2) : "";
                    break;
                }
                
                auto id = it2->second;

                auto it3 = languageDescriptor.strings.find(id);
                if (it3 == languageDescriptor.strings.end())
                {
                    replacement = descriptorDebug ? buildErrorForString(id) : "";
                    break;
                }
                
                const auto& pvterm = languageDescriptor.pvterms.at(specifier.term);
                const auto& pluralForm = languageDescriptor.pluralForms.at(pvterm.pcategory);

                size_t i = pluralForm.pick(x);
                if (i == pluralForm.pluralUnits.size())
                {
                    replacement = descriptorDebug ? buildErrorForNumberPterm(x) : "";
                    break;
                }
                
                auto types = it3->second.typesForVariantCategories(pvterm.vcategories);
                if (types.empty())
                {
                    replacement = descriptorDebug ? buildErrorForStringVariant(id) : "";
                    break;
                }
                
                auto it4 = std::find_if(pvterm.variants.begin(), pvterm.variants.end(),
                [&] (const auto& variant) { return variant.ptype == i && variant.vtypes == types; });
                
                replacement = it4->string;
            } break;
        }
        
        builtString.insert(addedBytes + specifier.byteLocation, replacement);
        addedBytes += replacement.size();
    }
    
    return builtString;
}

std::string LocalizationManager::getFontName() const
{
    if (!languageDescriptor.fontName.empty())
        return languageDescriptor.fontName;
    return DefaultFontName;
}

LocalizationManager::CallbackEntry LocalizationManager::registerLanguageChangeCallback(LocalizationManager::Callback callback)
{
    return CallbackEntry(languageChangeCallbacks, languageChangeCallbacks.insert(languageChangeCallbacks.end(), callback));
}

std::string languageDescriptorForLocale(std::string locale)
{
    for (const auto& name : getAllLanguageDescriptors())
    {
        if (name == DefaultLanguageDescriptor) continue;
        
        LanguageDescriptor descriptor;
        sf::FileInputStream file;
        if (!file.open(getPathOfLanguageDescriptor(name)) ||
            !readFromStream(file, descriptor)) continue;
        
#if _WIN32
        auto compareLocale = descriptor.windowsLocale;
#else
        auto compareLocale = descriptor.posixLocale;
#endif
        
        if (locale.find(compareLocale) != std::string::npos)
            return name;
    }
    
    return DefaultLanguageDescriptor;
}
