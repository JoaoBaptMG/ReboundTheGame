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


#include <iostream>
#include <unordered_map>
#include "tinyxml2.h"
#include "varlength.hpp"
#include "object-writers-helpers.hpp"

using namespace std;
using namespace tinyxml2;

bool doNothing(Object obj, ostream&)
{
    return obj.everythingOk();
}

bool writeBox(Object obj, ostream& out)
{
    obj.assertType(Object::Type::Rectangle);
    
    int16_t width = obj.getWidth();
    int16_t height = obj.getHeight();
    
    out.write((const char*)&width, sizeof(int16_t));
    out.write((const char*)&height, sizeof(int16_t));
    
    return obj.everythingOk();
}

bool write_enemies_bosses_TestBoss(Object obj, ostream& out)
{
    int16_t leftSpan = obj.getPropertyInt("left-span");
    int16_t rightSpan = obj.getPropertyInt("right-span");
    
    out.write((const char*)&leftSpan, sizeof(int16_t));
    out.write((const char*)&rightSpan, sizeof(int16_t));
    
    return obj.everythingOk();
}

bool write_background_Parallax(Object obj, ostream& out)
{
    auto file = obj.getPropertyFile("name");
    auto factor = obj.getPropertyFloat("factor");
    
    write_varlength(out, file.size());
    out.write(file.data(), file.size() * sizeof(char));
    out.write((const char*)&factor, sizeof(factor));
    
    return obj.everythingOk();
}

bool write_collectibles_GoldenToken(Object obj, ostream& out)
{
    uint8_t id = obj.getPropertyInt("id");
    out.write((const char*)&id, sizeof(id));
    return obj.everythingOk();
}

bool write_collectibles_HealthPickup(Object obj, ostream& out)
{
    uint16_t id = obj.getPropertyInt("amount");
    out.write((const char*)&id, sizeof(id));
    return obj.everythingOk();
}

bool write_collectibles_Powerup(Object obj, ostream& out)
{
    uint8_t id = obj.getPropertyInt("level");
    out.write((const char*)&id, sizeof(id));
    return obj.everythingOk();
}

bool write_props_Water(Object obj, ostream& out)
{
    obj.assertType(Object::Type::Rectangle);
    
    int16_t width = obj.getWidth();
    int16_t height = obj.getHeight();
    
    out.write((const char*)&width, sizeof(int16_t));
    out.write((const char*)&height, sizeof(int16_t));
    
    uint8_t hideTop = obj.getPropertyBool("hide-top", true);
    uint32_t coastColor = obj.getPropertyColor("coast-color", true, 0xFFFFFFFF);
    uint32_t color = obj.getPropertyColor("color", true, 0xFFFF6464);

    out.write((const char*)&hideTop, sizeof(uint8_t));
    out.write((const char*)&coastColor, sizeof(uint32_t));
    out.write((const char*)&color, sizeof(uint32_t));
    
    return obj.everythingOk();
}

bool write_props_MessageSign(Object obj, ostream& out)
{
    string message = obj.getPropertyString("message", true, "");
    if (message.empty())
    {
        string formatter = obj.getPropertyString("message-formatter");
        if (!obj.everythingOk()) return false;

        char v = 2;
        write_varlength(out, formatter.size()+1);
        out.write((const char*)&v, sizeof(char));
        out.write(formatter.data(), formatter.size() * sizeof(char));
    }
    else
    {
        write_varlength(out, message.size());
        out.write(message.data(), message.size() * sizeof(char));
    }

    return obj.everythingOk();
}

bool write_props_SwitchingBlock(Object obj, ostream& out)
{
    string name = obj.getPropertyString("parent-cluster");
    size_t id = obj.getPropertyInt("cluster-time");

    write_varlength(out, name.size());
    out.write(name.data(), name.size() * sizeof(char));
    write_varlength(out, id);

    return obj.everythingOk();
}

bool write_props_SwitchingBlockCluster(Object obj, ostream& out)
{
    size_t numBlockTimes = obj.getPropertyInt("total-times");
    size_t numVisibleTimes = obj.getPropertyInt("visible-times");
    float durationSeconds = obj.getPropertyFloat("duration");

    write_varlength(out, numBlockTimes);
    write_varlength(out, numVisibleTimes);
    out.write((const char*)&durationSeconds, sizeof(float));

    return obj.everythingOk();
}

bool write_enemies_Hopper(Object obj, ostream& out)
{
    uint8_t facingRight = obj.getPropertyBool("facing-right", true, false);
    out.write((const char*)&facingRight, sizeof(uint8_t));
    
    return obj.everythingOk();
}

unordered_map<string,bool(*)(Object,ostream&)> objectWriters =
{
    { "Player", doNothing },
    
    { "background::Parallax", write_background_Parallax },
    
    { "collectibles::GoldenToken", write_collectibles_GoldenToken },
    { "collectibles::HealthPickup", write_collectibles_HealthPickup },
    { "collectibles::Powerup", write_collectibles_Powerup },
    
    { "enemies::Floater", doNothing },
    { "enemies::Rotator", writeBox },
    { "enemies::Hopper", write_enemies_Hopper },
    
    { "enemies::bosses::TestBoss", write_enemies_bosses_TestBoss },
    
    { "props::BombCrate", doNothing },
    { "props::DashCrate", doNothing },
    { "props::Grapple", doNothing },
    { "props::PushableCrate", writeBox },
    { "props::Water", write_props_Water },
    { "props::MessageSign", write_props_MessageSign },
    { "props::SwitchingBlock", write_props_SwitchingBlock },
    { "props::SwitchingBlockCluster", write_props_SwitchingBlockCluster },
};
