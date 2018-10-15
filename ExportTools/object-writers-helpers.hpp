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
