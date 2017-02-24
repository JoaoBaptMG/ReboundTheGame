#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include "tinyxml2.h"

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

	ofstream out(outFile);
	out.write("LEVEL", 5);

    auto lvl = docHandle.FirstChildElement("level");
    
    uint16_t number = lvl.ToElement()->UnsignedAttribute("number");
    auto songName = lvl.ToElement()->Attribute("song");
    uint32_t songNameS = strlen(songName);

    out.write((const char*)&number, sizeof(uint16_t));

    uint16_t startingRoom = (uint16_t)-1;
    auto startingRoomPos = out.tellp();
    out.write((const char*)&startingRoom, sizeof(uint16_t));

    out.write((const char*)&songNameS, sizeof(uint32_t));
    out.write(songName, songNameS * sizeof(char));

    uint32_t maxId = 0;
    for (auto rm = lvl.FirstChildElement("room"); rm.ToElement(); rm = rm.NextSiblingElement("room"))
    {
        uint32_t id = rm.ToElement()->UnsignedAttribute("id");
        if (maxId < id) maxId = id;
    }

    maxId++;
    out.write((const char*)&maxId, sizeof(uint32_t));   

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
            out.write((const char*)&nameS, sizeof(uint32_t));
            out.write(name, nameS * sizeof(char));
        }
        else
        {
            nameS = 0;
            out.write((const char*)&nameS, sizeof(uint32_t));
        }
    }

    out.seekp(startingRoomPos);
    out.write((const char*)&startingRoom, sizeof(uint16_t));

    return 0;
}
