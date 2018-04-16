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
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <unordered_map>
#include "object-writers-helpers.hpp"
#include "tinyxml2.h"
#include "varlength.hpp"

#if _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

using namespace std;
using namespace tinyxml2;

extern unordered_map<string,bool(*)(Object,ostream&)> objectWriters;

auto findObjectLayer(XMLHandle map, const char* name)
{
    for (auto objgroup = map.FirstChildElement("objectgroup"); objgroup.ToElement();
        objgroup = objgroup.NextSiblingElement("objectgroup"))
        if (objgroup.ToElement()->Attribute("name", name))
            return objgroup;
    
    return XMLHandle(nullptr);
}

auto nextcsv(const char* ch)
{
    auto ptr = strchr(ch, ',');
    return ptr ? ptr+1 : nullptr;
}

int tmxToMap(string inFile, string outFile)
{
	XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return -1;
	}

	XMLHandle docHandle(doc);

	ofstream out(outFile, ios::out | ios::binary);
	out.write("ROOM", 4);

	auto map = docHandle.FirstChildElement("map");
	auto tset = map.FirstChildElement("tileset");

	auto maxTile = tset.ToElement()->UnsignedAttribute("tilecount")-1;
	auto firstId = tset.ToElement()->UnsignedAttribute("firstgid");

	const char* tilesetName = nullptr;
	auto props = map.FirstChildElement("properties");
    for (auto prop = props.FirstChildElement("property"); prop.ToElement(); prop = prop.NextSiblingElement("property"))
    {
        if (prop.ToElement()->Attribute("name", "tileset"))
        {
            tilesetName = prop.ToElement()->Attribute("value");
            break;
        }
    }

	if (tilesetName)
	{
        auto s = strlen(tilesetName);
		write_varlength(out, s);
		out.write(tilesetName, s * sizeof(char));
	}
    else write_varlength(out, 0);

	uint32_t width, height;
	auto layer = map.FirstChildElement("layer");
	if (layer.ToElement())
	{
		width = layer.ToElement()->UnsignedAttribute("width");
		height = layer.ToElement()->UnsignedAttribute("height");

		write_varlength(out, width);
        write_varlength(out, height);

		auto data = layer.FirstChildElement("data");
		auto txt = data.FirstChild().ToText()->Value();

		while (isspace(*txt)) txt++;

		unsigned long writtenTiles = 0;
		for (auto ch = txt; ch; ch = nextcsv(ch))
		{
			uint8_t v = (uint8_t)strtoul(ch, nullptr, 10)-firstId; // NOT UB!
			if (v > maxTile) v = maxTile;
			out.write((const char*)&v, sizeof(uint8_t));

			writtenTiles++;
		}
	}

    uint32_t objTotal = 0;
    stringstream objects(stringstream::binary | stringstream::out);
    
    auto objgroup = findObjectLayer(map, "object-layer");
    for (auto obj = objgroup.FirstChildElement("object"); obj.ToElement(); obj = obj.NextSiblingElement("object"))
    {
        auto elm = obj.ToElement();

        auto typeStr = elm->Attribute("type");
        uint32_t tsize = strlen(typeStr);
        write_varlength(objects, tsize);
        objects.write(typeStr, tsize * sizeof(char));
        
        auto nameStr = elm->Attribute("name");
        uint32_t nsize = strlen(nameStr);
        write_varlength(objects, nsize);
        objects.write(nameStr, nsize * sizeof(char));

        stringstream curObj(stringstream::binary | stringstream::out);

        int16_t posX = (int16_t)(elm->FloatAttribute("x") + 0.5f * elm->FloatAttribute("width"));
        int16_t posY = (int16_t)(elm->FloatAttribute("y") + 0.5f * elm->FloatAttribute("height"));
        curObj.write((const char*)&posX, sizeof(int16_t));
        curObj.write((const char*)&posY, sizeof(int16_t));
        
        auto it = objectWriters.find(typeStr);
        if (it == objectWriters.end())
        {
            cout << "Unrecognized object type " << typeStr << " of object " << nameStr << '.' << endl;
            return false;
        }
        if (!it->second(Object(elm), curObj))
        {
            cout << "Failed to write object " << nameStr << '.' << endl;
            return false;
        }

        objects << curObj.str();
        objTotal++;
    }

    write_varlength(out, objTotal);
    out << objects.str();
    objects.clear();

    auto warps = findObjectLayer(map, "warp-layer");

    uint32_t maxId = (uint32_t)-1;
    for (auto obj = warps.FirstChildElement("object"); obj.ToElement(); obj = obj.NextSiblingElement("object"))
    {
        auto warp = obj.ToElement();
        uint16_t id = (uint16_t)strtoul(warp->Attribute("name"), nullptr, 10);
        if (maxId == (uint32_t)-1 || maxId < id) maxId = id;
    }

    maxId++;
    write_varlength(out, maxId);
    auto globalSizePos = out.tellp();
    
    for (uint32_t i = 0; i < maxId; i++)
    {
        int16_t values[4] = { -1, -1, -1, -1 };
        out.write((const char*)values, 4*sizeof(int16_t));
    }

    for (auto obj = warps.FirstChildElement("object"); obj.ToElement(); obj = obj.NextSiblingElement("object"))
    {
        auto warp = obj.ToElement();

        auto warpDir = warp->Attribute("type");
        int dir = -1;

        if (!strcasecmp(warpDir, "right")) dir = 0;
        else if (!strcasecmp(warpDir, "down")) dir = 1;
        else if (!strcasecmp(warpDir, "left")) dir = 2;
        else if (!strcasecmp(warpDir, "up")) dir = 3;

        if (dir == -1) continue;

        auto warpNameCode = warp->Attribute("name");
        uint16_t id = (uint16_t)strtoul(warpNameCode, nullptr, 10);

        auto warpRoomDest = strstr(warpNameCode, "->");
        if (warpRoomDest) warpRoomDest += 2;
        else continue;

        uint16_t roomId = (uint16_t)strtoul(warpRoomDest, nullptr, 10);

        auto warpIdDest = strchr(warpRoomDest, '.');
        if (warpIdDest) warpIdDest += 1;
        else continue;

        uint16_t warpId = (uint16_t)strtoul(warpIdDest, nullptr, 10) | (dir << 14);
        
        int16_t c1, c2;

        if (dir & 1)
        {
            c1 = warp->IntAttribute("x");
            c2 = c1 + warp->IntAttribute("width");
        }
        else
        {
            c1 = warp->IntAttribute("y");
            c2 = c1 + warp->IntAttribute("height");
        }

        out.seekp(globalSizePos + streamoff(4*sizeof(int16_t) * id));
        out.write((const char*)&c1, sizeof(int16_t));
        out.write((const char*)&c2, sizeof(int16_t));
        out.write((const char*)&roomId, sizeof(uint16_t));
        out.write((const char*)&warpId, sizeof(uint16_t));
    }

    return 0;
}
