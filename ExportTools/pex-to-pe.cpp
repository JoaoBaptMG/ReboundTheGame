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
#include <limits>
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

#pragma pack(push, 1)
template <typename T>
struct twovals { T first, second; };

struct ParticleEmitterStruct
{
    enum class Style : uint8_t { Smooth, Disk } particleStyle;
    float totalLifetime;
    float emissionPeriod;

    struct pt { float x, y; } emissionCenter, emissionHalfSize, acceleration;
    float emissionInnerLimit;

    struct triple { float first, second, weight = 1.0; }
    direction, speed, sizeBegin, sizeEnd;
    
    twovals<float> lifetime;
    twovals<uint32_t> colorBegin, colorEnd;
    uint8_t generateHSV;
};
#pragma pack(pop)

static bool read(XMLElement* el, float& val)
{
    return el->QueryFloatAttribute("value", &val) == XML_SUCCESS;
}

static bool read(XMLElement* el, uint8_t& val)
{
    bool d;
    if (el->QueryBoolAttribute("value", &d) != XML_SUCCESS) return false;
    val = d;
    return true;
}

static bool read(XMLElement* el, ParticleEmitterStruct::triple& val)
{
    if (el->QueryFloatAttribute("value", &val.first) == XML_SUCCESS)
    {
        val.second = val.first;
        val.weight = 1.0;
        return true;
    }
    else
    {
        val.weight = el->FloatAttribute("weight", 1.0);
        
        return el->QueryFloatAttribute("first", &val.first) == XML_SUCCESS &&
               el->QueryFloatAttribute("second", &val.second) == XML_SUCCESS;
    }
}

static bool read(XMLElement* el, twovals<float>& val)
{
    ParticleEmitterStruct::triple dummy;
    if (read(el, dummy))
    {
        val.first = dummy.first;
        val.second = dummy.second;

        return true;
    }

    return false;
}

static bool read(XMLElement* el, twovals<uint32_t>& vals)
{
    auto val = el->Attribute("value");
    
    if (val) vals.first = vals.second = strtoul(val+1, nullptr, 16);
    else
    {
        auto fa = el->Attribute("first");
        auto sa = el->Attribute("second");

        vals.first = fa ? strtoul(fa+1, nullptr, 16) : 0;
        vals.second = sa ? strtoul(sa+1, nullptr, 16) : 0;

        if (!(fa && sa)) return false;
    }

    vals.first = ((vals.first >> 24) & 0xFF) | ((vals.first >> 8) & 0xFF00) |
                 ((vals.first << 8) & 0xFF0000) | ((vals.first << 24) & 0xFF000000);
    vals.second = ((vals.second >> 24) & 0xFF) | ((vals.second >> 8) & 0xFF00) |
                  ((vals.second << 8) & 0xFF0000) | ((vals.second << 24) & 0xFF000000);

    return true;
}

static bool read(XMLElement* el, ParticleEmitterStruct::Style& style)
{
    auto val = el->Attribute("value");
    
    if (!val) return false;
    else
    {
        if (!strcasecmp(val, "smooth")) style = ParticleEmitterStruct::Style::Smooth;
        else if (!strcasecmp(val, "disk")) style = ParticleEmitterStruct::Style::Disk;
        else style = ParticleEmitterStruct::Style::Disk;
    }

    return true;
}

static bool read(XMLElement* el, ParticleEmitterStruct::pt& val)
{
    return el->QueryFloatAttribute("x", &val.x) == XML_SUCCESS &&
           el->QueryFloatAttribute("y", &val.y) == XML_SUCCESS;
}

int pexToPe(string inFile, string outFile)
{
    XMLDocument doc;
	if (doc.LoadFile(inFile.c_str()) != XML_SUCCESS)
	{
		cout << "Error while trying to read file " << inFile << ": " << doc.ErrorName() << "." << endl;
		return -1;
	}

    XMLHandle docHandle(doc);

    ofstream out(outFile, ios::out | ios::binary);
    out.write("PEMIT", 5);

    unordered_map<std::string,ParticleEmitterStruct> emitters;
    emitters.emplace("", ParticleEmitterStruct());

    auto readParam = [&emitters](XMLHandle emitter, const char* pname, const char* name,
        const char* extends, auto ptr)
    {
        auto el = emitter.FirstChildElement(pname).ToElement();
        if (!(el && read(el, emitters[name].*ptr)))
            emitters[name].*ptr = emitters[extends].*ptr;
    };

    auto emitterSet = docHandle.FirstChildElement("particle-emitter-set");

    for (auto emitter = emitterSet.FirstChildElement("particle-emitter"); emitter.ToElement();
              emitter = emitter.NextSiblingElement("particle-emitter"))
    {
        auto name = emitter.ToElement()->Attribute("name");
        auto extends = emitter.ToElement()->Attribute("extends");
        if (!extends || emitters.find(extends) == emitters.end()) extends = "";

        readParam(emitter, "particle-style", name, extends, &ParticleEmitterStruct::particleStyle);
        readParam(emitter, "total-lifetime", name, extends, &ParticleEmitterStruct::totalLifetime);
        readParam(emitter, "emission-period", name, extends, &ParticleEmitterStruct::emissionPeriod);
        readParam(emitter, "emission-center", name, extends, &ParticleEmitterStruct::emissionCenter);
        readParam(emitter, "emission-half-size", name, extends, &ParticleEmitterStruct::emissionHalfSize);
        readParam(emitter, "acceleration", name, extends, &ParticleEmitterStruct::acceleration);
        readParam(emitter, "emission-inner-limit", name, extends, &ParticleEmitterStruct::emissionInnerLimit);
        readParam(emitter, "direction", name, extends, &ParticleEmitterStruct::direction);
        readParam(emitter, "speed", name, extends, &ParticleEmitterStruct::speed);
        readParam(emitter, "size-begin", name, extends, &ParticleEmitterStruct::sizeBegin);
        readParam(emitter, "size-end", name, extends, &ParticleEmitterStruct::sizeEnd);
        readParam(emitter, "lifetime", name, extends, &ParticleEmitterStruct::lifetime);
        readParam(emitter, "color-begin", name, extends, &ParticleEmitterStruct::colorBegin);
        readParam(emitter, "color-end", name, extends, &ParticleEmitterStruct::colorEnd);
        readParam(emitter, "generate-hsv", name, extends, &ParticleEmitterStruct::generateHSV);
    }

    write_varlength(out, emitters.size());
    for (const auto& pair : emitters)
    {
        write_varlength(out, pair.first.size());
        out.write(pair.first.c_str(), pair.first.size() * sizeof(char));
        out.write((const char*)&pair.second, sizeof(ParticleEmitterStruct));
    }
    
    return 0;
}
