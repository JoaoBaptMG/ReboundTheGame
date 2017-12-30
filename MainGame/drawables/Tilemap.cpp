#include "Tilemap.hpp"
#include <cmath>

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mutableUpdateVertexMap(states.transform);

    states.texture = texture.get();
    states.transform.translate((float)tileSize * lastPoint.x, (float)tileSize * lastPoint.y);
    target.draw(vertices.get(), vertexSize, sf::Quads, states);
}

void Tilemap::mutableUpdateVertexMap(sf::Transform transform) const
{
    auto invTransform = transform.getInverse();
    auto targetFrame = invTransform.transformRect(drawingFrame);

    size_t width = (size_t)floorf((targetFrame.left + targetFrame.width)/tileSize) -
                   (size_t)floorf((targetFrame.left)/tileSize) + 1;
    size_t height = (size_t)floorf((targetFrame.top + targetFrame.height)/tileSize) -
                    (size_t)floorf((targetFrame.top)/tileSize) + 1;

    bool dirty = false;

    if (vertexSize != 4*width*height)
    {
        vertexSize = 4*width*height;
        vertices.reset(new sf::Vertex[vertexSize]);
        lastPoint = sf::Vector2i(0, 0);
        dirty = true;
    }

    sf::Vector2i pt(size_t(targetFrame.left/tileSize), size_t(targetFrame.top/tileSize));

    if (dirty || lastPoint != pt)
    {
        size_t stride = texture->getSize().x / tileSize;

        for (size_t j = 0; j < height; j++)
            for (size_t i = 0; i < width; i++)
            {
                auto cur = pt + sf::Vector2i(i, j);

                if (cur.x < 0 || cur.y < 0 || cur.x >= (intmax_t)tileData.width() || cur.y >= (intmax_t)tileData.height())
                {
                    for (size_t k = 0; k < 4; k++)
                        vertices[(j*width+i)*4+k].color = sf::Color(0, 0, 0, 0);
                    
                    vertices[(j*width+i)*4+0].position = sf::Vector2f(0, 0);
                    vertices[(j*width+i)*4+1].position = sf::Vector2f(0, 0);
                    vertices[(j*width+i)*4+2].position = sf::Vector2f(0, 0);
                    vertices[(j*width+i)*4+3].position = sf::Vector2f(0, 0);
                    
                    vertices[(j*width+i)*4+0].texCoords = sf::Vector2f(0, 0);
                    vertices[(j*width+i)*4+1].texCoords = sf::Vector2f(0, 0);
                    vertices[(j*width+i)*4+2].texCoords = sf::Vector2f(0, 0);
                    vertices[(j*width+i)*4+3].texCoords = sf::Vector2f(0, 0);
                }
                else
                {
                    size_t data = tileData(cur.x, cur.y);
                    size_t texS = data % stride;
                    size_t texT = data / stride;

                    for (size_t k = 0; k < 4; k++)
                        vertices[(j*width+i)*4+k].color = sf::Color::White;

                    vertices[(j*width+i)*4+0].position = sf::Vector2f((float)i*tileSize, (float)j*tileSize);
                    vertices[(j*width+i)*4+1].position = sf::Vector2f((float)(i+1)*tileSize, (float)j*tileSize);
                    vertices[(j*width+i)*4+2].position = sf::Vector2f((float)(i+1)*tileSize, (float)(j+1)*tileSize);
                    vertices[(j*width+i)*4+3].position = sf::Vector2f((float)i*tileSize, (float)(j+1)*tileSize);

                    vertices[(j*width+i)*4+0].texCoords = (float)tileSize * sf::Vector2f(texS, texT);
                    vertices[(j*width+i)*4+1].texCoords = (float)tileSize * sf::Vector2f(texS+1, texT);
                    vertices[(j*width+i)*4+2].texCoords = (float)tileSize * sf::Vector2f(texS+1, texT+1);
                    vertices[(j*width+i)*4+3].texCoords = (float)tileSize * sf::Vector2f(texS, texT+1);
                }
            }

        lastPoint = pt;
    }
}
