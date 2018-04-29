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
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <memory>
#include "tinyxml2.h"
#include "varlength.hpp"

using namespace std;
using namespace tinyxml2;

int lvxToLvl(string inFile, string outFile)
{
    XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return -1;
	}

	XMLHandle docHandle(doc);

	ofstream out(outFile, ios::out | ios::binary);
	out.write("LEVEL", 5);

    auto lvl = docHandle.FirstChildElement("level");
    
    uint16_t number = lvl.ToElement()->UnsignedAttribute("number");
    auto songName = lvl.ToElement()->Attribute("song");
    uint32_t songNameS = strlen(songName);

    out.write((const char*)&number, sizeof(uint16_t));

    uint16_t startingRoom = (uint16_t)-1;
    auto startingRoomPos = out.tellp();
    out.write((const char*)&startingRoom, sizeof(uint16_t));

    auto colorVal = lvl.ToElement()->Attribute("map-color");
    if (colorVal == nullptr)
    {
        cout << "Attribute map-color is required!" << endl;
        return -1;
    }

    if (colorVal[0] != '#')
    {
        cout << "Attribute map-color must a color of the form #rrggbb" << endl;
        return -1;
    }

    uint32_t color = strtoull(colorVal+1, nullptr, 16);
    uint32_t red = color & 0xFF0000;
    uint32_t blue = color & 0xFF;
    color = (color & 0xFF00FF00) | 0xFF000000 | (red >> 16) | (blue << 16);
    out.write((const char*)&color, sizeof(uint32_t));

    write_varlength(out, songNameS);
    out.write(songName, songNameS * sizeof(char));

    uint32_t maxId = 0;
    for (auto rm = lvl.FirstChildElement("room"); rm.ToElement(); rm = rm.NextSiblingElement("room"))
    {
        uint32_t id = rm.ToElement()->UnsignedAttribute("id");
        if (maxId < id) maxId = id;
    }

    maxId++;
    write_varlength(out, maxId);  

    unique_ptr<const char*[]> names{ new const char*[maxId] };

    for (auto rm = lvl.FirstChildElement("room"); rm.ToElement(); rm = rm.NextSiblingElement("room"))
    {
        uint16_t id = rm.ToElement()->UnsignedAttribute("id");
        names[id] = rm.ToElement()->Attribute("name");

        if (rm.ToElement()->BoolAttribute("starting"))
            startingRoom = id;
    }

    for (uint32_t i = 0; i < maxId; i++)
    {
        const char* name = names[i];
        uint32_t nameS;
        if (name)
        {
            nameS = strlen(name);
            write_varlength(out, nameS);
            out.write(name, nameS * sizeof(char));
        }
        else write_varlength(out, 0);
    }

    write_varlength(out, 0);

    out.seekp(startingRoomPos);
    out.write((const char*)&startingRoom, sizeof(uint16_t));

    return 0;
}
