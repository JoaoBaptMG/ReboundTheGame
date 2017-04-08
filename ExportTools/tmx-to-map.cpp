#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include "tinyxml2.h"
#include "varlength.hpp"

using namespace std;
using namespace tinyxml2;

auto findObjectLayer(XMLHandle map, const char* name)
{
    for (auto objgroup = map.FirstChildElement("objectgroup"); objgroup.ToElement();
        objgroup = objgroup.NextSiblingElement("objectgroup"))
        if (objgroup.ToElement()->Attribute("name", name))
            return objgroup;
    
    return XMLHandle(nullptr);
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

	ofstream out(outFile);
	out.write("ROOM", 4);

	auto map = docHandle.FirstChildElement("map");
	auto tset = map.FirstChildElement("tileset");

	const char* imgStr = nullptr;
	auto img = tset.FirstChildElement("image");
	if (img.ToElement()) imgStr = img.ToElement()->Attribute("source");

	if (imgStr)
	{
        auto str = strrchr(imgStr, '/');
        if (str) imgStr = str+1;

        auto s = strlen(imgStr);
		write_varlength(out, s);
		out.write(imgStr, s * sizeof(char));
	}
    else write_varlength(out, 0);

	uint32_t width, height;
	auto layer = map.FirstChildElement("layer");
	if (layer.ToElement())
	{
		width = atol(layer.ToElement()->Attribute("width"));
		height = atol(layer.ToElement()->Attribute("height"));

		write_varlength(out, width);
        write_varlength(out, height);

		auto data = layer.FirstChildElement("data");
		auto txt = data.FirstChild().ToText()->Value();

		while (isspace(*txt)) txt++;

		for (auto ch = txt; ch-1; ch = strchr(ch, ',')+1)
		{
			uint8_t v = atoi(ch)-1;
            if (v > 13) v = 13;
			out.write((const char*)&v, sizeof(uint8_t));
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

        uint32_t strSize = 2 * sizeof(int16_t);
        stringstream curObj(stringstream::binary | stringstream::out);

        int16_t posX = elm->FloatAttribute("x") + 0.5f * elm->FloatAttribute("width");
        int16_t posY = elm->FloatAttribute("y") + 0.5f * elm->FloatAttribute("height");
        curObj.write((const char*)&posX, sizeof(int16_t));
        curObj.write((const char*)&posY, sizeof(int16_t));

        auto props = obj.FirstChildElement("properties");
        for (auto prop = props.FirstChildElement("property"); prop.ToElement(); prop = prop.NextSiblingElement("property"))
        {
            auto pelm = prop.ToElement();

            const char* str;
            if (!(str = pelm->Attribute("value")))
                str = pelm->GetText();

            if (pelm->Attribute("type", "int"))
            {
                int32_t val = atol(str);
                curObj.write((const char*)&val, sizeof(int32_t));
                strSize += sizeof(int32_t);
            }
            else if (pelm->Attribute("type", "float"))
            {
                float val = strtof(str, nullptr);
                curObj.write((const char*)&val, sizeof(float));
                strSize += sizeof(float);
            }
            else if (pelm->Attribute("type", "bool"))
            {
                bool val = !strcmp(str, "true");
                curObj.write((const char*)&val, sizeof(bool));
                strSize += sizeof(bool);
            }
            else if (pelm->Attribute("type", "color"))
            {
                uint32_t val = strtoul(str+1, nullptr, 16);
                curObj.write((const char*)&val, sizeof(uint32_t));
                strSize += sizeof(uint32_t);
            }
            else
            {
                if (pelm->Attribute("type", "file"))
                {
                    auto cstr = strrchr(str, '/');
                    if (cstr) str = cstr+1;
                }

                if (!strcasecmp(str, "#width"))
                {
                    int16_t width = elm->IntAttribute("width");
                    curObj.write((const char*)&width, sizeof(int16_t));
                    strSize += sizeof(int16_t);
                }
                else if (!strcasecmp(str, "#height"))
                {
                    int16_t height = elm->IntAttribute("height");
                    curObj.write((const char*)&height, sizeof(int16_t));
                    strSize += sizeof(int16_t);
                }
                else
                {
                    uint32_t size = strlen(str);
                    auto s = write_varlength(curObj, size);
                    curObj.write(str, size * sizeof(char));

                    strSize += s + size * sizeof(char);
                }
            }
        }

        write_varlength(objects, strSize);
        objects << curObj.str();
        objTotal++;
    }

    write_varlength(out, objTotal);
    out << objects.str();
    objects.clear();

    auto warps = findObjectLayer(map, "warp-layer");

    uint32_t maxId = 0;
    for (auto obj = warps.FirstChildElement("object"); obj.ToElement(); obj = obj.NextSiblingElement("object"))
    {
        auto warp = obj.ToElement();
        uint16_t id = atol(warp->Attribute("name"));
        if (maxId < id) maxId = id;
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
        uint16_t id = atol(warpNameCode);

        auto warpRoomDest = strstr(warpNameCode, "->");
        if (warpRoomDest) warpRoomDest += 2;
        else continue;

        uint16_t roomId = atol(warpRoomDest);

        auto warpIdDest = strchr(warpRoomDest, '.');
        if (warpIdDest) warpIdDest += 1;
        else continue;

        uint16_t warpId = atol(warpIdDest) | (dir << 14);
        
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
