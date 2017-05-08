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

    out.seekp(startingRoomPos);
    out.write((const char*)&startingRoom, sizeof(uint16_t));

    return 0;
}
