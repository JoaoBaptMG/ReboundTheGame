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


#include "GUIMap.hpp"

#include <map>

#include <assert.hpp>
#include <chronoUtils.hpp>
#include <rect.hpp>
#include "defaults.hpp"
#include "gameplay/MapGenerator.hpp"
#include "data/LevelData.hpp"
#include "ScissorRectUtils.hpp"

#include <iostream>

// Optimization
struct MapTextureData
{
    sf::VertexArray vertArray;
    Texture texture;
    MapTextureData() : vertArray(sf::PrimitiveType::Triangles) {}
};
using WeakLvlPtr = std::weak_ptr<LevelData>;
static std::map<WeakLvlPtr,MapTextureData,std::owner_less<WeakLvlPtr>> staticLevelTextures;
void clearMapTextures() { staticLevelTextures.clear(); }

constexpr float BlinkPeriod = 2;

const util::rect MapViewport(-54, -54, 108, 108);
const util::rect ExtendedMapViewport(-364, -204, 728, 408);

constexpr uint8_t PresentSpeed = 6;

void setColorOnly(glm::u8vec4& color, glm::u8vec4 dest)
{
    color.r = dest.r;
    color.g = dest.g;
    color.b = dest.b;
}

util::rect GUIMap::getBounds() const
{
    return extendedFrame ? ExtendedMapViewport : MapViewport;
}

void GUIMap::update(FrameTime curTime)
{
    if (initTime == decltype(initTime)()) initTime = curTime;
    
    float t = toSeconds<float>(curTime - initTime);
    float weight = 0.5 - 0.5 * cosf(2 * M_PI * t / BlinkPeriod);
    glm::u8vec4 finalColor(255 + weight * ((int)mapBlinkColor.r - 255),
                         255 + weight * ((int)mapBlinkColor.g - 255),
                         255 + weight * ((int)mapBlinkColor.b - 255),
                         255 + weight * ((int)mapBlinkColor.a - 255));
                        
    if (curLevel)
        for (size_t i = 0; i < curLevel->roomMaps.size(); i++)
            for (size_t k = 0; k < 6; k++)
            {
                setColorOnly(vertArray[6*i+k].color, curRoom == i ? finalColor : Colors::White);
                if (vertArray[6*i+k].color.a != 0 && vertArray[6*i+k].color.a != 255)
                    vertArray[6*i+k].color.a = std::min((size_t)vertArray[6*i+k].color.a + PresentSpeed, (size_t)255);
            }
}

void GUIMap::setCurLevel(std::shared_ptr<LevelData> level)
{
    if (curLevel != level)
    {
        curLevel = level;

        mapBlinkColor.r = (uint8_t)((255 + (uint16_t)level->mapColor.r) / 2);
        mapBlinkColor.g = (uint8_t)((255 + (uint16_t)level->mapColor.g) / 2);
        mapBlinkColor.b = (uint8_t)((255 + (uint16_t)level->mapColor.b) / 2);
        mapBlinkColor.a = 255;

        buildLevelTexture();
    }
}

void GUIMap::buildLevelTexture()
{
    if (!curLevel)
    {
        vertArray.clear();
        return;
    }
    
    for (auto it = staticLevelTextures.begin(); it != staticLevelTextures.end();)
        if (it->first.expired()) it = staticLevelTextures.erase(it);
        else ++it;

    
    auto it = staticLevelTextures.find(curLevel);
    if (it != staticLevelTextures.end())
    {
        mapTexture = &it->second.texture;
        vertArray = it->second.vertArray;
    }
    else
    {
        auto& data = staticLevelTextures.emplace(curLevel, MapTextureData()).first->second;
        mapTexture = &data.texture;
        
        util::irect bounds;
        data.vertArray.resize(6 * curLevel->roomMaps.size());

        for (const auto& mapData : curLevel->roomMaps)
            bounds = bounds.unite(util::irect(mapData.x, mapData.y, mapData.map.width(), mapData.map.height()));
                
        ASSERT(mapTexture->create(bounds.width, bounds.height));

        auto buildVertex = [=](float x, float y)
        {
            return sf::Vertex(sf::Vector2f(x, y), glm::u8vec4(255, 255, 255, 0), sf::Vector2f(x - bounds.x, y - bounds.y));
        };

        size_t i = 0;
        for (const auto& mapData : curLevel->roomMaps)
        {
            if (mapData.map.empty()) continue;
            mapTexture->update(getTextureData(mapData.map).get(), mapData.map.width(), mapData.map.height(),
                mapData.x - bounds.x, mapData.y - bounds.y);
            
            data.vertArray[i++] = buildVertex(mapData.x, mapData.y);
            data.vertArray[i++] = buildVertex(mapData.x + (int16_t)mapData.map.width(), mapData.y);
            data.vertArray[i++] = buildVertex(mapData.x + (int16_t)mapData.map.width(), mapData.y + (int16_t)mapData.map.height());
            data.vertArray[i++] = buildVertex(mapData.x, mapData.y);
            data.vertArray[i++] = buildVertex(mapData.x + (int16_t)mapData.map.width(), mapData.y + (int16_t)mapData.map.height());
            data.vertArray[i++] = buildVertex(mapData.x, mapData.y + (int16_t)mapData.map.height());
        }
        
        vertArray = data.vertArray;
    }
}

void GUIMap::presentRoom(size_t room)
{
    if (!curLevel) return;
    
    for (size_t k = 0; k < 6; k++)
        if (vertArray[6*room+k].color.a == 0)
            vertArray[6*room+k].color.a = PresentSpeed;
}
    
void GUIMap::presentRoomFull(size_t room)
{
    if (!curLevel) return;
    
    for (size_t k = 0; k < 6; k++)
        vertArray[6*room+k].color.a = 255;
}

void GUIMap::hideRoom(size_t room)
{
    if (!curLevel) return;
    
    for (size_t k = 0; k < 6; k++)
        vertArray[6*room+k].color.a = 0;
}

void GUIMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!curLevel) return;
    
    ScissorRectGuard guard{states.transform.transformRect(getBounds())};
    
    states.transform.translate(-curLevel->roomMaps.at(curRoom).x, -curLevel->roomMaps.at(curRoom).y);
    states.transform.translate(-displayPosition.x, -displayPosition.y);
    states.texture = mapTexture;
    
    target.draw(vertArray, states);
}
