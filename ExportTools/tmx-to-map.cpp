#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

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
		imgStr = strrchr(imgStr, '/')+1;
		
		uint32_t s = strlen(imgStr);
		out.write((const char*)&s, sizeof(uint32_t));
		out.write(imgStr, s * sizeof(char));
	}
    else
    {
        uint32_t z = 0;
        out.write((const char*)&z, sizeof(uint32_t));
    }

	uint32_t width, height;
	auto layer = map.FirstChildElement("layer");
	if (layer.ToElement())
	{
		width = atol(layer.ToElement()->Attribute("width"));
		height = atol(layer.ToElement()->Attribute("height"));

		out.write((const char*)&width, sizeof(uint32_t));
		out.write((const char*)&height, sizeof(uint32_t));

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

    return 0;
}
