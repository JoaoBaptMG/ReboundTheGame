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
#include <fstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <memory>
#include <vector>
#include <unordered_map>
#include "tinyxml2.h"
#include "varlength.hpp"

#if _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

using namespace std;
using namespace tinyxml2;

const unordered_map<string,size_t> Attributes =
{
    { "none", 0 },
    { "solid", 1 },
    { "no-walljump", 2 },
    { "spike", 3 },
    { "crumbling", 4 },
    { "left-solid", 5 },
    { "right-solid", 6 },
    { "up-solid", 7 },
    { "down-solid", 8 },
    { "left-no-walljump", 9 },
    { "right-no-walljumṕ", 10 }
};

constexpr auto AttributesHelpStr = "none, solid, no-walljump, spike, crumbling, "
    "left-solid, right-solid, up-solid, down-solid, left-no-walljump, right-no-walljumṕ";

const unordered_map<string,size_t> TileTypes =
{
    { "none", 0 },
    { "terrain-upper-left", 1 },
    { "terrain-up", 2 },
    { "terrain-upper-right", 3 },
    { "terrain-left", 4 },
    { "terrain-center", 5 },
    { "terrain-right", 6 },
    { "terrain-lower-left", 7 },
    { "terrain-down", 8 },
    { "terrain-lower-right", 9 },
    { "semi-terrain1", 10 },
    { "semi-terrain2", 11 },
    { "semi-terrain3", 12 },
    { "single-object", 13 }
};
constexpr auto TilesHelpStr = "none, terrain-upper-left, terrain-up, terrain-upper-right, "
    "terrain-left, terrain-center, terrain-right, terrain-down, terrain-lower-right, "
    "semi-terrain1, semi-terrain2, semi-terrain3, single-object";

const unordered_map<string,size_t> ShapeTypes =
{
    { "tile", 0 },
    { "circle", 1 },
    { "segment", 2 },
    { "polygon", 3 },
    { "box", 4 },
};

#pragma pack(push, 1)
struct terrain_params
{
    uint8_t attribute;
    int16_t physParams[5];
};
#pragma pack(pop)

struct single_object_params
{
    struct shape_params
    {
        uint8_t type;
        int16_t radius;
        vector<int16_t> points;
    };

    uint8_t attribute;
    float waitTime, crumbleTime;
    size_t crumblePieceSize;
    vector<shape_params> shapes;
};

bool loadTerrains(XMLHandle tset, string inFile, unordered_map<string,terrain_params>& terrains,
    unordered_map<string,single_object_params>& singleObjects)
{
    for (auto importer = tset.FirstChildElement("import-from"); importer.ToElement();
        importer = importer.NextSiblingElement("import-from"))
    {
        string file = importer.ToElement()->Attribute("file");

        auto pos = inFile.find_last_of("\\/");
        if (pos != string::npos)
            file = inFile.substr(0, pos+1) + file;

        XMLDocument subDoc;
        if (subDoc.LoadFile(file.c_str()) != XML_SUCCESS)
        {
            cout << "Error while trying to read file " << file;
            cout << " requested by " << inFile << ": " << subDoc.ErrorName() << "." << endl;
            return false;
        }

        XMLHandle subHandle(subDoc);
        if (!loadTerrains(subHandle.FirstChildElement("tileset"), file, terrains, singleObjects))
        {
            cout << "Error while loading imported file " << file << " requested by " << inFile << "." << endl;
            return false;
        }
    }

    for (auto terrain = tset.FirstChildElement("terrain"); terrain.ToElement();
        terrain = terrain.NextSiblingElement("terrain"))
    {
        auto telem = terrain.ToElement();
        
        auto name = telem->Attribute("name");
        if (!name)
        {
            cout << "Warning: nameless terrain ignored!" << endl;
            continue;
        }

        if (terrains.find(name) != terrains.end())
        {
            cout << "Error: terrain " << name << " being redefined." << endl;
            return false;
        }

        auto attribute = telem->Attribute("attribute");
        if (!attribute)
        {
            cout << "Error: terrain " << name << " must define an attribute (solid, no-walljump)." << endl;
            return false;
        }

        auto attrId = Attributes.find(attribute);
        if (attrId == Attributes.end() || !(attrId->second == 1 || attrId->second == 2))
        {
            cout << "Error: invalid attribute " << attribute << " for terrain " << name;
            cout << " (valid attributes: solid, no-walljump)." << endl;
            return false;
        }

        auto& terr = terrains[name];
        terr.attribute = (uint8_t)attrId->second;
        
        terr.physParams[0] = telem->IntAttribute("upper-offset");
        terr.physParams[1] = telem->IntAttribute("lower-offset");
        terr.physParams[2] = telem->IntAttribute("left-offset");
        terr.physParams[3] = telem->IntAttribute("right-offset");
        terr.physParams[4] = telem->IntAttribute("corner-radius");
    }

    for (auto object = tset.FirstChildElement("single-object"); object.ToElement();
        object = object.NextSiblingElement("single-object"))
    {
        auto oelem = object.ToElement();

        auto name = oelem->Attribute("name");
        if (!name)
        {
            cout << "Warning: nameless single object ignored!" << endl;
            continue;
        }

        if (singleObjects.find(name) != singleObjects.end())
        {
            cout << "Error: single object " << name << " being redefined." << endl;
            return false;
        }

        auto attribute = oelem->Attribute("attribute");
        if (!attribute)
        {
            cout << "Error: single object " << name << " must define an attribute";
            cout << " (" << AttributesHelpStr << ")." << endl;
            return false;
        }

        auto attrId = Attributes.find(attribute);
        if (attrId == Attributes.end())
        {
            cout << "Error: invalid attribute " << attribute << " for single object " << name;
            cout << " (valid attributes: " << AttributesHelpStr << ")." << endl;
            return false;
        }

        auto& obj = singleObjects[name];
        obj.attribute = (uint8_t)attrId->second;

        if (attrId->second == 4)
        {
            auto crumbling = object.FirstChildElement("crumbling").ToElement();
            if (!crumbling || crumbling != object.LastChildElement("crumbling").ToElement())
            {
                cout << "Error: single object" << name << " with crumbling attribute ";
                cout << "must provide a single crumbling subnode." << endl;
                return false;
            }

            obj.waitTime = crumbling->FloatAttribute("wait-time");
            obj.crumbleTime = crumbling->FloatAttribute("crumble-time", NAN);

            if (isnan(obj.crumbleTime))
            {
                cout << "Error: single object" << name << " with crumbling attribute ";
                cout << "must provide a valid crumble-time." << endl;
                return false;
            }

            obj.crumblePieceSize = crumbling->IntAttribute("crumble-piece-size", 8);
        }
        else if (object.FirstChildElement("crumbling").ToElement())
        {
            cout << "Error: single object" << name << " with non-crumbling attribute ";
            cout << "must not provide a crumbling subnode." << endl;
            return false;
        }

        for (auto shape = object.FirstChildElement("shape"); shape.ToElement();
            shape = shape.NextSiblingElement("shape"))
        {
            auto selem = shape.ToElement();
            
            auto type = selem->Attribute("type");
            if (!type)
            {
                cout << "Error: single object " << name << "'s shape must have a type";
                cout << " (tile, circle, segment, polygon, box)." << endl;
                return false;
            }

            auto typeId = ShapeTypes.find(type);
            if (typeId == ShapeTypes.end())
            {
                cout << "Error: invalid type " << type << " for single object " << name << "'s shape";
                cout << " (valid attributes: tile, circle, segment, polygon, box)." << endl;
                return false;
            }

            single_object_params::shape_params shp;
            shp.type = (uint8_t)typeId->second;

            auto radius = oelem->IntAttribute("radius");
            if (radius < 0 || (typeId->second == 1 && radius == 0))
            {
                cout << "Error: provide a valid radius for single object ";
                const char* names[] = { "tile", "circle", "segment", "polygon", "box" };
                cout << name << "'s " << names[typeId->second] << " shape." << endl;
                return false;
            }
            shp.radius = radius;

            if (typeId->second == 2 || typeId->second == 3)
            {
                for (auto point = shape.FirstChildElement("point"); point.ToElement();
                    point = point.NextSiblingElement("point"))
                {
                    auto x = point.ToElement()->IntAttribute("x");
                    auto y = point.ToElement()->IntAttribute("y");
                    shp.points.push_back(x);
                    shp.points.push_back(y);
                }

                if (typeId->second == 2 && shp.points.size() != 4)
                {
                    cout << "Error: provide exactly two points for single object " << name << "'s segment shape." << endl;
                    return false;
                }
                else if (shp.points.size() == 0)
                    cout << "Warning: single object " << name << "'s polyhgon shape does not have any points." << endl;
            }
            else if (typeId->second == 4)
            {
                auto width = selem->IntAttribute("width");
                auto height = selem->IntAttribute("height");

                if (width == 0 || height == 0)
                {
                    cout << "Error: provide a valid width and height for single object " << name << "'s box shape." << endl;
                    return false;
                }

                int x, y; 

                if (selem->Attribute("cx") && selem->Attribute("x"))
                {
                    cout << "Error: do not provide both x and cx attributes for single object " << name << "'s box shape." << endl;
                    return false;
                }
                if (selem->QueryIntAttribute("cx", &x) == XML_SUCCESS) x -= width/2;
                else x = selem->IntAttribute("x");

                if (selem->Attribute("cy") && selem->Attribute("y"))
                {
                    cout << "Error: do not provide both y and cy attributes for single object " << name << "'s box shape." << endl;
                    return false;
                }
                if (selem->QueryIntAttribute("cy", &y) == XML_SUCCESS) y -= height/2;
                else y = selem->IntAttribute("y");

                shp.points.push_back(x);
                shp.points.push_back(y);
                shp.points.push_back(x+width);
                shp.points.push_back(y+height);
            }
            
            obj.shapes.push_back(shp);
        }
        
        if (obj.shapes.empty())
            cout << "Warning: single object " << name << " does not have any shapes." << endl;
    }

    return true;
}

int tsxToTs(string inFile, string outFile)
{
    XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return -1;
	}

    XMLHandle docHandle(doc);

    ofstream out(outFile, ios::out | ios::binary);
    out.write("TSET", 4);

    auto tset = docHandle.FirstChildElement("tileset");
    if (tset.ToElement()->Attribute("only-includes", "true"))
        return 0;

    auto textureName = tset.ToElement()->Attribute("texture");
    if (textureName)
    {
        auto s = strlen(textureName);
		write_varlength(out, s);
		out.write(textureName, s * sizeof(char));
    }
    else write_varlength(out, 0);

    unordered_map<string,terrain_params> terrains;
    unordered_map<string,single_object_params> singleObjects;

    if (!loadTerrains(tset, inFile, terrains, singleObjects))
    {
        cout << "Error while parsing terrain data for " << inFile << "." << endl;
        return -1;
    }

    write_varlength(out, terrains.size());
    for (const auto& pair : terrains)
        out.write((const char*)&pair.second, sizeof(terrain_params));

    write_varlength(out, singleObjects.size());
    for (const auto& pair : singleObjects)
    {
        const auto& object = pair.second;

        out.write((const char*)&object.attribute, sizeof(uint8_t));
        if (object.attribute == 4)
        {
            out.write((const char*)&object.waitTime, sizeof(float));
            out.write((const char*)&object.crumbleTime, sizeof(float));
            write_varlength(out, object.crumblePieceSize);
        }
        
        write_varlength(out, object.shapes.size());
        for (const auto& shape : object.shapes)
        {
            uint8_t type = shape.type;
            if (type == 4) type = 3;
            out.write((const char*)&type, sizeof(uint8_t));
            
            switch (shape.type)
            {
                case 0: case 1: break;
                case 2: out.write((const char*)shape.points.data(), 4*sizeof(int16_t)); break;
                case 3:
                    write_varlength(out, shape.points.size()/2);
                    out.write((const char*)shape.points.data(), shape.points.size()*sizeof(int16_t));
                    break;
                case 4:
                    write_varlength(out, 4);
                    int16_t pts[8] =
                    {
                        shape.points[0], shape.points[1],
                        shape.points[2], shape.points[1],
                        shape.points[2], shape.points[3],
                        shape.points[0], shape.points[3]
                    };
                    out.write((const char*)pts, sizeof(pts));
                    break;
            }

            out.write((const char*)&shape.radius, sizeof(int16_t));
        }
    }

    uint32_t maxId = 0;
    for (auto tobj = tset.FirstChildElement("tile"); tobj.ToElement(); tobj = tobj.NextSiblingElement("tile"))
    {
        auto obj = tobj.ToElement();
        uint32_t id = obj->UnsignedAttribute("id");
        if (maxId < id) maxId = id;
    }

    maxId++;
    write_varlength(out, maxId);

    struct tile_params
    {
        uint8_t type = 0;
        size_t id = 0;
    };

    unique_ptr<tile_params[]> tileModes { new tile_params[maxId] };

    for (auto tobj = tset.FirstChildElement("tile"); tobj.ToElement(); tobj = tobj.NextSiblingElement("tile"))
    {
        auto obj = tobj.ToElement();
        auto id = obj->UnsignedAttribute("id");

        // We'll get back here in a minute
        auto type = obj->Attribute("type");
        if (!type)
        {
            cout << "Error: tile " << id << " must define a type";
            cout << " (" << TilesHelpStr << ")." << endl;
            return -1;
        }

        auto typeId = TileTypes.find(type);
        if (typeId == TileTypes.end())
        {
            cout << "Error: invalid type " << type << " for tile " << id;
            cout << " (valid types: " << TilesHelpStr << ")." << endl;
            return -1;
        }

        tileModes[id].type = (uint8_t)typeId->second;

        if (typeId->second >= 1 && typeId->second <= 9)
        {
            auto name = obj->Attribute("name");
            if (!name)
            {
                cout << "Error: provide a terrain name for tile " << id << "." << endl;
                return -1;
            }

            auto terrain = terrains.find(name);
            if (terrain == terrains.end())
            {
                cout << "Error: terrain name " << name << " not found for tile " << id << "." << endl;
                return -1;
            }

            tileModes[id].id = distance(terrains.begin(), terrain);
        }
        else if (typeId->second >= 10 && typeId->second <= 12)
        {
            auto name = obj->Attribute("name");
            if (!name)
            {
                cout << "Error: provide a semi-terrain object name for tile " << id << "." << endl;
                return -1;
            }

            auto semiTerrainId = Attributes.find(name);
            if (semiTerrainId == Attributes.end() || !(semiTerrainId->second >= 5 || semiTerrainId->second <= 10))
            {
                cout << "Error: invalid semi-terrain name " << name << " for tile " << id << "." << endl;
                return -1;
            }

            tileModes[id].id = semiTerrainId->second - 5;
        }
        else if (typeId->second == 13)
        {
            auto name = obj->Attribute("name");
            if (!name)
            {
                cout << "Error: provide a single object name for tile " << id << "." << endl;
                return -1;
            }

            auto object = singleObjects.find(name);
            if (object == singleObjects.end())
            {
                cout << "Error: single object name " << name << " not found for tile " << id << "." << endl;
                return -1;
            }

            tileModes[id].id = distance(singleObjects.begin(), object);
        }
    }

    for (size_t i = 0; i < maxId; i++)
    {
        out.write((const char*)&tileModes[i].type, sizeof(uint8_t));
        write_varlength(out, tileModes[i].id);
    }

    return 0;
}
