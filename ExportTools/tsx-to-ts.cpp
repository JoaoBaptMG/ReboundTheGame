#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>
#include "tinyxml2.h"
#include "varlength.hpp"

using namespace std;
using namespace tinyxml2;

int tsxToTs(string inFile, string outFile)
{
    XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return -1;
	}

    XMLHandle docHandle(doc);

    ofstream out(outFile);
    out.write("TSET", 4);

    auto tset = docHandle.FirstChildElement("tileset");

    auto textureName = tset.ToElement()->Attribute("texture");
    if (textureName)
    {
        auto s = strlen(textureName);
		write_varlength(out, s);
		out.write(textureName, s * sizeof(char));
    }
    else write_varlength(out, 0);

    int16_t physicalProperties[5] = { 0, 0, 0, 0, 0 };

    auto params = tset.FirstChildElement("physical-parameters").ToElement();
    if (params)
    {
        physicalProperties[0] = params->IntAttribute("upper-offset");
        physicalProperties[1] = params->IntAttribute("lower-offset");
        physicalProperties[2] = params->IntAttribute("left-offset");
        physicalProperties[3] = params->IntAttribute("right-offset");
        physicalProperties[4] = params->IntAttribute("corner-radius");
    }

    out.write((const char*)physicalProperties, 5 * sizeof(int16_t));

    uint32_t maxId = 0;
    for (auto tobj = tset.FirstChildElement("tile"); tobj.ToElement(); tobj = tobj.NextSiblingElement("tile"))
    {
        auto obj = tobj.ToElement();
        uint32_t id = obj->UnsignedAttribute("id");
        if (maxId < id) maxId = id;
    }

    maxId++;
    write_varlength(out, maxId);

    unique_ptr<uint8_t[]> tileModes { new uint8_t[maxId] };

    for (auto tobj = tset.FirstChildElement("tile"); tobj.ToElement(); tobj = tobj.NextSiblingElement("tile"))
    {
        auto obj = tobj.ToElement();
        auto id = obj->UnsignedAttribute("id");

        const char* attr = obj->Attribute("mode");

        uint8_t mode = 0;

        if (attr)
        {
            if (!strncasecmp(attr, "solid", 5)) mode = 1;
            else if (!strncasecmp(attr, "water", 5)) mode = 2;
        }

        tileModes[id] = mode;
    }

    out.write((const char*)tileModes.get(), maxId * sizeof(uint8_t));

    return 0;
}
