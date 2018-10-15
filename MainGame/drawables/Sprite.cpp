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

#include "Sprite.hpp"

#include <assert.hpp>

constexpr auto SpriteFragmentShader = R"fragment(
uniform sampler2D tex;
uniform vec3 multColor, addColor;
uniform float opacity, grayscaleFactor;

void main()
{
    gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);
    gl_FragColor.rgb = addColor + multColor * gl_FragColor.rgb;
    
    float grayscale = dot(gl_FragColor.rgb, vec3(0.299, 0.587, 0.114));
    gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(grayscale), grayscaleFactor);
    
    gl_FragColor.a *= opacity;
}
)fragment";

static inline auto toVec3(glm::u8vec4 color)
{
    return sf::Glsl::Vec3(color.r/255.0f, color.g/255.0f, color.b/255.0f);
}

sf::Shader& Sprite::getSpriteShader()
{
    static sf::Shader shader;
    static bool shaderLoaded = false;

    if (!shaderLoaded)
    {
        ASSERT(shader.loadFromMemory(SpriteFragmentShader, sf::Shader::Fragment));
        shader.setUniform("tex", sf::Shader::CurrentTexture);
        shaderLoaded = true;
    }

    return shader;
}

Sprite::Sprite(std::shared_ptr<Texture> texture, glm::vec2 anchor) : texture(texture), anchorPoint(anchor),
    blendColor(Colors::White), flashColor(glm::u8vec4(0, 0, 0, 0)), opacity(1), grayscaleFactor(0),
    vertices(sf::PrimitiveType::TriangleFan)
{
    texRect = util::rect(glm::vec2(0, 0), glm::vec2(getTextureSize().x, getTextureSize().y));
    vertices.resize(4);
    setupVertices();
}

Sprite::Sprite(std::shared_ptr<Texture> texture) : Sprite(texture, glm::vec2(texture->getSize().x/2.0f, texture->getSize().y/2.0f)) {}

Sprite::Sprite() : Sprite(nullptr, glm::vec2(0, 0)) {}

util::rect Sprite::getBounds() const
{
    util::rect bounds(texRect);
    bounds.x -= anchorPoint.x;
    bounds.y -= anchorPoint.y;
    return bounds;
}

void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!texture) return;

    auto blend = toVec3(blendColor);
    auto flash = toVec3(flashColor);

    auto& shader = getSpriteShader();
    shader.setUniform("multColor", (1 - flashColor.a/255.0f) * blend);
    shader.setUniform("addColor", flashColor.a/255.0f * flash);
    shader.setUniform("opacity", opacity);
    shader.setUniform("grayscaleFactor", grayscaleFactor);
    
    states.blendMode = sf::BlendAlpha;
    states.transform.translate(-anchorPoint.x, -anchorPoint.y);
    states.texture = texture.get();
    states.shader = &shader;

    target.draw(vertices, states);
}

void Sprite::setupVertices()
{
    for (size_t i = 0; i < vertices.getVertexCount(); i++)
        vertices[i].color = Colors::White;

    vertices[0].position = vertices[0].texCoords = sf::Vector2f(texRect.x, texRect.y);
    vertices[1].position = vertices[1].texCoords = sf::Vector2f(texRect.x + texRect.width, texRect.y);
    vertices[2].position = vertices[2].texCoords = sf::Vector2f(texRect.x + texRect.width, texRect.y + texRect.height);
    vertices[3].position = vertices[3].texCoords = sf::Vector2f(texRect.x, texRect.y + texRect.height);
}
