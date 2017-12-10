#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "tinyxml2.h"

struct pt { float x, y; };

class Object final
{
public:
    enum class Type { Rectangle, Ellipse, Polygon, Polyline };
    struct Property
    {
        enum class Type { Bool, Color, Float, File, Int, String } type;
        std::string value;
    };

private:
    Type type;
    pt position;
    float width, height;
    std::string name;
    std::vector<pt> points;
    std::unordered_map<std::string,Property> properties;
    bool failed;
    
public:
    Object(tinyxml2::XMLElement* decodee);
    
    void assertType(Type type);
    std::string getProperty(std::string name, Property::Type type, bool optional, bool* exists = nullptr);
    
    auto getType() const { return type; }
    auto getPosition() const { return position; }
    auto getWidth() const { return width; }
    auto getHeight() const { return height; }
    auto getCenter() const { return pt{ position.x + width/2, position.y + height/2 }; }
    const auto& getPoints() const { return points; }
    
    bool getPropertyBool(std::string name, bool optional = false, bool defaultValue = false);
    uint32_t getPropertyColor(std::string name, bool optional = false, uint32_t defaultValue = 0);
    float getPropertyFloat(std::string name, bool optional = false, float defaultValue = 0);
    std::string getPropertyFile(std::string name, bool optional = false, std::string defaultValue = "");
    int64_t getPropertyInt(std::string name, bool optional = false, int64_t defaultValue = 0);
    std::string getPropertyString(std::string name, bool optional = false, std::string defaultValue = "");
    
    auto everythingOk() const { return !failed; }
};
