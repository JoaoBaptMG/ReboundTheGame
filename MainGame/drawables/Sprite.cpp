#include "Sprite.hpp"

#include "utility/assert.hpp"

constexpr auto SpriteFragmentShader = R"fragment(
uniform sampler2D tex;
uniform vec3 multColor, addColor;
uniform float opacity;

void main()
{
    gl_FragColor = texture2D(tex, gl_TexCoord[0].xy);
    gl_FragColor.rgb = addColor + multColor * gl_FragColor.rgb;
    gl_FragColor.a *= opacity;
}
)fragment";

static inline auto toVec3(sf::Color color)
{
    return sf::Glsl::Vec3(color.r/255.0, color.g/255.0, color.b/255.0);
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
    blendColor(sf::Color::White), flashColor(sf::Color(0, 0, 0, 0)), opacity(1)
{
    setupVertices();
}

Sprite::Sprite(std::shared_ptr<sf::Texture> texture) : Sprite(texture, sf::Vector2f(texture->getSize())/2.0f)
{
}

sf::FloatRect Sprite::getTextureBounds() const
{
    return sf::FloatRect(-anchorPoint, (sf::Vector2f)getTextureSize());
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
    
    states.blendMode = sf::BlendAlpha;
    states.transform.translate(-anchorPoint);
    states.texture = texture.get();
    states.shader = &shader;

    target.draw(quad, 4, sf::TriangleFan, states);
}

void Sprite::setupVertices()
{
    for (auto& vtx : quad) vtx.color = sf::Color::White;

    auto size = getTextureSize();

    quad[0].position = quad[0].texCoords = sf::Vector2f(0, 0);
    quad[1].position = quad[1].texCoords = sf::Vector2f(size.x, 0);
    quad[2].position = quad[2].texCoords = sf::Vector2f(size.x, size.y);
    quad[3].position = quad[3].texCoords = sf::Vector2f(0, size.y);
}
