#include "object-writers-helpers.hpp"

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#if _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

using namespace std;
using namespace tinyxml2;

static auto nameForType(Object::Type type)
{
    switch (type)
    {
        case Object::Type::Rectangle: return "rectangle";
        case Object::Type::Ellipse: return "ellipse";
        case Object::Type::Polygon: return "polygon";
        case Object::Type::Polyline: return "polyline";
    }
}

static auto nameForType(Object::Property::Type type)
{
    switch (type)
    {
        case Object::Property::Type::Bool: return "bool";
        case Object::Property::Type::Color: return "color";
        case Object::Property::Type::Float: return "float";
        case Object::Property::Type::File: return "file";
        case Object::Property::Type::Int: return "int";
        case Object::Property::Type::String: return "string";
    }
}

Object::Object(tinyxml2::XMLElement* element) : type(Type::Rectangle), failed(false)
{
    position.x = element->FloatAttribute("x");
    position.y = element->FloatAttribute("y");
    
    width = element->FloatAttribute("width");
    height = element->FloatAttribute("height");
    
    XMLElement* el;
    if ((el = element->FirstChildElement("ellipse")))
        type = Type::Ellipse;
    else if ((el = element->FirstChildElement("polygon")))
        type = Type::Polygon;
    else if ((el = element->FirstChildElement("polyline")))
        type = Type::Polyline;
        
    if (type == Type::Polygon || type == Type::Polyline)
    {
        const char* pchar = el->Attribute("points");
        
        while (pchar && *pchar)
        {
            pt cur;
            cur.x = strtof(pchar, nullptr);
            pchar = strchr(pchar, ',');
            
            if (!pchar)
            {
                cout << "Invalid point string while trying to read object " << element->Attribute("name") << ".\n";
                failed = true;
                break;
            }
            
            cur.y = strtof(pchar+1, nullptr);
            pchar = strchr(pchar, ' ');
            
            if (!pchar)
            {
                cout << "Invalid point string while trying to read object " << element->Attribute("name") << ".\n";
                failed = true;
                break;
            }
            
            points.push_back(cur);
            pchar++;
        }
    }
    
    auto props = element->FirstChildElement("properties");
    if (props)
    {
        for (auto pr = props->FirstChildElement("property"); pr; pr = pr->NextSiblingElement("property"))
        {
            string name = pr->Attribute("name");
            Property property;
            
            property.value = pr->Attribute("value");
            auto tystr = pr->Attribute("type");
            
            if (!strcasecmp(tystr, "bool"))
                property.type = Property::Type::Bool;
            else if (!strcasecmp(tystr, "color"))
                property.type = Property::Type::Color;
            else if (!strcasecmp(tystr, "float"))
                property.type = Property::Type::Float;
            else if (!strcasecmp(tystr, "file"))
                property.type = Property::Type::File;
            else if (!strcasecmp(tystr, "int"))
                property.type = Property::Type::Int;
            else property.type = Property::Type::String;
            
            properties.emplace(name, property);
        }
    }
    
    name = element->Attribute("name");
}

void Object::assertType(Object::Type type)
{
    if (this->type != type)
    {
        cout << "Unexpected object shape " << nameForType(this->type) << " of object " << name;
        cout << " (expecting " << nameForType(type) << ")." << endl;
        failed = true;
    }
}

std::string Object::getProperty(std::string name, Object::Property::Type type, bool optional, bool* exists)
{
    auto it = properties.find(name);
    
    if (exists) *exists = true;
    
    if (it == properties.end())
    {
        if (!optional)
        {
            cout << "Required property " << name << " not found on object " << this->name << '.' << endl;
            failed = true;
        }
        else if (exists) *exists = false;
        return "";
    }
    
    if (it->second.type != type)
    {
        cout << "Unexpected property type " << nameForType(it->second.type) << " of property " << name;
        cout << "on object " << this->name << " (expecting " << nameForType(type) << ")." << endl;
        failed = true;
        return "";
    }
    
    return it->second.value;
}

bool Object::getPropertyBool(string name, bool optional, bool defaultValue)
{
    auto value = getProperty(name, Property::Type::Bool, optional);
    if (value == "") return false;
    if (value != "true" && value != "false")
    {
        cout << "Unrecognized value " << value << " of bool property " << name << " on object " << this->name;
        cout << " (expecting true or false)." << endl;
        failed = false;
        return false;
    }
    return value == "true";
}

uint32_t Object::getPropertyColor(string name, bool optional, uint32_t defaultValue)
{
    auto value = getProperty(name, Property::Type::Color, optional);
    if (value == "") return defaultValue;
    if (value[0] != '#')
    {
        cout << "Unrecognized value " << value << " of color property " << name << " on object " << this->name;
        cout << " (expecting a color in formar ##aarrggbb)." << endl;
        failed = false;
        return false;
    }
    
    uint32_t val = strtoul(&value[1], nullptr, 16);
    uint32_t red = val & 0x00ff0000;
    uint32_t blue = val & 0x0000ff;
    return (val & 0xff00ff00) | (red >> 16) | (blue << 16);
}

float Object::getPropertyFloat(string name, bool optional, float defaultValue)
{
    auto value = getProperty(name, Property::Type::Float, optional);
    if (value == "") return defaultValue;
    return strtof(value.data(), nullptr);
}

std::string Object::getPropertyFile(string name, bool optional, std::string defaultValue)
{
    bool exists = false;
    auto prop = getProperty(name, Property::Type::File, optional, &exists);
    return exists ? prop : defaultValue;
}

int64_t Object::getPropertyInt(string name, bool optional, int64_t defaultValue)
{
    auto value = getProperty(name, Property::Type::Int, optional);
    if (value == "") return 0;
    return strtoll(value.data(), nullptr, 10);
}

std::string Object::getPropertyString(string name, bool optional, std::string defaultValue)
{
    bool exists = false;
    auto prop = getProperty(name, Property::Type::String, optional, &exists);
    return exists ? prop : defaultValue;
}



