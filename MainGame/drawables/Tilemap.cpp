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


#include "Tilemap.hpp"
#include <cmath>

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mutableUpdateVertexMap(states.transform);

    if (vertexSize > 0)
    {
        states.texture = texture.get();
        states.transform.translate((float)tileSize * lastPoint.x, (float)tileSize * lastPoint.y);
        target.draw(vertices.get(), vertexSize, sf::Triangles, states);
    }
}

util::rect Tilemap::getTextureRectForTile(size_t tile) const
{
    if (tile == (uint8_t)-1) return util::rect{0, 0, 0, 0};
    size_t stride = texture->getSize().x / tileSize;
    size_t texS = tile % stride;
    size_t texT = tile / stride;

    return util::rect{(float)tileSize * texS, (float)tileSize * texT, (float)tileSize, (float)tileSize};
}

void Tilemap::mutableUpdateVertexMap(sf::Transform transform) const
{
    auto invTransform = transform.getInverse();
    
    util::rect tilemapFrame(0, 0, tileSize*tileData.width(), tileSize*tileData.height());
    util::rect targetFrame = invTransform.transformRect(drawingFrame);
    if (!targetFrame.intersects(tilemapFrame))
    {
        vertexSize = 0;
        vertices.reset();
        return;
    }
    
    targetFrame = targetFrame.intersect(tilemapFrame);

    size_t width = (size_t)floorf((targetFrame.x + targetFrame.width)/tileSize) -
                   (size_t)floorf((targetFrame.x)/tileSize) + 1;
    size_t height = (size_t)floorf((targetFrame.y + targetFrame.height)/tileSize) -
                    (size_t)floorf((targetFrame.y)/tileSize) + 1;

    bool dirty = false;

    if (vertexSize != 6*width*height)
    {
        vertexSize = 6*width*height;
        vertices.reset(new sf::Vertex[vertexSize]);
        lastPoint = glm::ivec2(0, 0);
        dirty = true;
    }

    glm::ivec2 pt(size_t(targetFrame.x/tileSize), size_t(targetFrame.y/tileSize));

    if (dirty || lastPoint != pt)
    {
        size_t stride = texture->getSize().x / tileSize;

        for (size_t j = 0; j < height; j++)
            for (size_t i = 0; i < width; i++)
            {
                auto cur = pt + glm::ivec2(i, j);

                if (cur.x < 0 || cur.y < 0 || cur.x >= (intmax_t)tileData.width()
                    || cur.y >= (intmax_t)tileData.height() || tileData(cur.x, cur.y) == (uint8_t)-1)
                {
                    for (size_t k = 0; k < 6; k++)
                    {
                        vertices[(j*width+i)*6+k].color = glm::u8vec4(0, 0, 0, 0);
                        vertices[(j*width+i)*6+k].position = sf::Vector2f(0, 0);
                        vertices[(j*width+i)*6+k].texCoords = sf::Vector2f(0, 0);
                    }
                }
                else
                {
                    size_t data = tileData(cur.x, cur.y);
                    size_t texS = data % stride;
                    size_t texT = data / stride;

                    for (size_t k = 0; k < 6; k++)
                        vertices[(j*width+i)*6+k].color = Colors::White;

                    vertices[(j*width+i)*6+0].position = sf::Vector2f((float)i*tileSize, (float)j*tileSize);
                    vertices[(j*width+i)*6+1].position = sf::Vector2f((float)(i+1)*tileSize, (float)j*tileSize);
                    vertices[(j*width+i)*6+2].position = sf::Vector2f((float)(i+1)*tileSize, (float)(j+1)*tileSize);
                    vertices[(j*width+i)*6+3].position = sf::Vector2f((float)i*tileSize, (float)(j+1)*tileSize);
                    vertices[(j*width+i)*6+4].position = vertices[(j*width+i)*6+0].position;
                    vertices[(j*width+i)*6+5].position = vertices[(j*width+i)*6+2].position;

                    vertices[(j*width+i)*6+0].texCoords = (float)tileSize * sf::Vector2f(texS, texT);
                    vertices[(j*width+i)*6+1].texCoords = (float)tileSize * sf::Vector2f(texS+1, texT);
                    vertices[(j*width+i)*6+2].texCoords = (float)tileSize * sf::Vector2f(texS+1, texT+1);
                    vertices[(j*width+i)*6+3].texCoords = (float)tileSize * sf::Vector2f(texS, texT+1);
                    vertices[(j*width+i)*6+4].texCoords = vertices[(j*width+i)*6+0].texCoords;
                    vertices[(j*width+i)*6+5].texCoords = vertices[(j*width+i)*6+2].texCoords;
                }
            }

        lastPoint = pt;
    }
}
