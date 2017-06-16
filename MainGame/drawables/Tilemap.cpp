#include "Tilemap.hpp"
#include <cmath>

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    mutableUpdateVertexMap(states.transform);

    states.texture = texture.get();
    states.transform.translate(long(tileSize) * lastPoint.x, long(tileSize) * lastPoint.y);
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

        for (size_t j = 0; j < height; j++)
            for (size_t i = 0; i < width; i++)
            {
                vertices[(j*width+i)*4+0].color = sf::Color::White;
                vertices[(j*width+i)*4+0].position = sf::Vector2f(i*tileSize, j*tileSize);

                vertices[(j*width+i)*4+1].color = sf::Color::White;
                vertices[(j*width+i)*4+1].position = sf::Vector2f((i+1)*tileSize, j*tileSize);

                vertices[(j*width+i)*4+2].color = sf::Color::White;
                vertices[(j*width+i)*4+2].position = sf::Vector2f((i+1)*tileSize, (j+1)*tileSize);

                vertices[(j*width+i)*4+3].color = sf::Color::White;
                vertices[(j*width+i)*4+3].position = sf::Vector2f(i*tileSize, (j+1)*tileSize);
            }
    }

    sf::Vector2i pt(size_t(targetFrame.left/tileSize), size_t(targetFrame.top/tileSize));

    if (dirty || lastPoint != pt)
    {
        size_t stride = texture->getSize().x / tileSize;

        for (size_t j = 0; j < height; j++)
            for (size_t i = 0; i < width; i++)
            {
                auto cur = pt + sf::Vector2i(i, j);

                if (cur.x < 0 || cur.y < 0 || cur.x >= tileData.width() || cur.y >= tileData.height())
                {
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

                    vertices[(j*width+i)*4+0].texCoords = sf::Vector2f(tileSize * texS, tileSize * texT);
                    vertices[(j*width+i)*4+1].texCoords = sf::Vector2f(tileSize * (texS+1), tileSize * texT);
                    vertices[(j*width+i)*4+2].texCoords = sf::Vector2f(tileSize * (texS+1), tileSize * (texT+1));
                    vertices[(j*width+i)*4+3].texCoords = sf::Vector2f(tileSize * texS, tileSize * (texT+1));
                }
            }

        lastPoint = pt;
    }
}
