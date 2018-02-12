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

static inline auto toVec3(sf::Color color)
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

Sprite::Sprite(std::shared_ptr<sf::Texture> texture, sf::Vector2f anchor) : texture(texture), anchorPoint(anchor),
    blendColor(sf::Color::White), flashColor(sf::Color(0, 0, 0, 0)), opacity(1), grayscaleFactor(0),
    vertices(sf::PrimitiveType::TriangleFan)
{
    texRect = sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(getTextureSize()));
    vertices.resize(4);
    setupVertices();
}

Sprite::Sprite(std::shared_ptr<sf::Texture> texture) : Sprite(texture, sf::Vector2f(texture->getSize())/2.0f) {}

Sprite::Sprite() : Sprite(nullptr, sf::Vector2f(0, 0)) {}

sf::FloatRect Sprite::getBounds() const
{
    sf::FloatRect bounds(texRect);
    bounds.left -= anchorPoint.x;
    bounds.top -= anchorPoint.y;
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
    states.transform.translate(-anchorPoint);
    states.texture = texture.get();
    states.shader = &shader;

    target.draw(vertices, states);
}

void Sprite::setupVertices()
{
    for (size_t i = 0; i < vertices.getVertexCount(); i++)
        vertices[i].color = sf::Color::White;

    vertices[0].position = vertices[0].texCoords = sf::Vector2f(texRect.left, texRect.top);
    vertices[1].position = vertices[1].texCoords = sf::Vector2f(texRect.left + texRect.width, texRect.top);
    vertices[2].position = vertices[2].texCoords = sf::Vector2f(texRect.left + texRect.width, texRect.top + texRect.height);
    vertices[3].position = vertices[3].texCoords = sf::Vector2f(texRect.left, texRect.top + texRect.height);
}
