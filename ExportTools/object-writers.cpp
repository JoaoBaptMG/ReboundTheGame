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

unordered_map<string,bool(*)(Object,ostream&)> objectWriters =
{
    { "Player", doNothing },
    
    { "background::Parallax", write_background_Parallax },
    
    { "collectibles::GoldenToken", write_collectibles_GoldenToken },
    { "collectibles::HealthPickup", write_collectibles_HealthPickup },
    { "collectibles::Powerup", write_collectibles_Powerup },
    
    { "enemies::Floater", doNothing },
    { "enemies::Rotator", writeBox },
    
    { "props::BombCrate", doNothing },
    { "props::DashCrate", doNothing },
    { "props::Grapple", doNothing },
    { "props::PushableCrate", writeBox },
    { "props::Water", writeBox },
};
