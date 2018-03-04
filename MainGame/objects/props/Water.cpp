#include "Water.hpp"

#include "objects/Player.hpp"
#include "scene/GameScene.hpp"
#include "rendering/Renderer.hpp"
#include <cmath>

#include <assert.hpp>

cpFloat catet(cpFloat h, cpFloat c) { ASSERT(h*h - c*c >= 0); return sqrt(h*h - c*c); }

cpFloat intersectQuarterCircle(cpBB bb, cpFloat radius)
{
    if (bb.l < 0) bb.l = 0;
    if (bb.b < 0) bb.b = 0;

    if (bb.r < 0 || bb.t < 0) return 0;
    if (cpvlengthsq({ bb.l, bb.b }) >= radius*radius) return 0;
    if (cpvlengthsq({ bb.r, bb.t }) <= radius*radius) return cpBBArea(bb);

    bool rout = cpvlengthsq({ bb.r, bb.b }) > radius*radius;
    bool tout = cpvlengthsq({ bb.l, bb.t }) > radius*radius;

    if (rout && tout)
    {
        cpFloat sr = catet(radius, bb.b) - bb.l;
        cpFloat st = catet(radius, bb.l) - bb.b;

        cpFloat angle = 1.57079632679 - asin(bb.b/radius) - asin(bb.l/radius);
        return (sr*st + (angle - sin(angle))*radius*radius)/2;
    }
    else if (rout)
    {
        cpFloat sb = catet(radius, bb.b) - bb.l;
        cpFloat st = catet(radius, bb.t) - bb.l;

        cpFloat angle = asin(bb.t/radius) - asin(bb.b/radius);
        return ((sb+st)*(bb.t-bb.b) + (angle - sin(angle))*radius*radius)/2;
    }
    else if (tout)
    {
        cpFloat sl = catet(radius, bb.l) - bb.b;
        cpFloat sr = catet(radius, bb.r) - bb.b;

        cpFloat angle = asin(bb.r/radius) - asin(bb.l/radius);
        return ((sl+sr)*(bb.r-bb.l) + (angle - sin(angle))*radius*radius)/2;
    }
    else
    {
        cpFloat sr = catet(radius, bb.t) - bb.l;
        cpFloat st = catet(radius, bb.r) - bb.b;

        cpFloat angle = 1.57079632679 - acos(bb.r/radius) - acos(bb.l/radius);
        
        return (sr*(bb.t-bb.b) + st*(bb.r-bb.l) + cpBBArea(bb) - sr*st +
                (angle - sin(angle))*radius*radius)/2;
    }
}

cpFloat intersectionAreaForAABBCircle(cpBB bb, cpVect pos, cpFloat radius)
{
    return intersectQuarterCircle({ bb.l - pos.x, bb.b - pos.y, bb.r - pos.x, bb.t - pos.y }, radius) +
           intersectQuarterCircle({ pos.x - bb.r, bb.b - pos.y, pos.x - bb.l, bb.t - pos.y }, radius) +
           intersectQuarterCircle({ bb.l - pos.x, pos.y - bb.t, bb.r - pos.x, pos.y - bb.b }, radius) +
           intersectQuarterCircle({ pos.x - bb.r, pos.y - bb.t, pos.x - bb.l, pos.y - bb.b }, radius);
}

using namespace props;

Water::Water(GameScene& scene) : GameObject(scene), oldArea(0), shape(sf::Vector2f(256, 256))
{
    shape.setColor(sf::Color(100, 100, 255, 128));
    shape.setCoastColor(sf::Color(255, 255, 255, 128));
}

Water::~Water()
{
     auto player = gameScene.getObjectByName<Player>("player");
     if (player) player->addToWaterArea(-oldArea);
}

bool Water::configure(const ConfigStruct& config)
{
    sf::IntRect destRect(config.position.x - config.width/2, config.position.y - config.height/2,
                         config.width, config.height);
    setRect(destRect);
    shape.setTopHidden(config.hideTop);
    
    return true;
}

void Water::setRect(sf::IntRect rect)
{
    shape.setDrawingSize(sf::Vector2f(rect.width, rect.height));

    this->rect = { (cpFloat)rect.left, (cpFloat)rect.top,
                   (cpFloat)(rect.left + rect.width), (cpFloat)(rect.top + rect.height) };
}

void Water::update(std::chrono::steady_clock::time_point curTime)
{
    auto player = gameScene.getObjectByName<Player>("player");

    if (player)
    {
        auto pos = player->getPosition(), vel = player->getVelocity();
        
        cpFloat area = intersectionAreaForAABBCircle(rect, pos, PlayerRadius);
        player->addToWaterArea(area - oldArea);
        oldArea = area;

        cpFloat waveGen = pos.y - rect.b;
        if (fabs(waveGen) <= 64.0)
        {
            for (intmax_t k = -32; k <= 32; k++)
                shape.setVelocity(pos.x + k - rect.l,
                    vel.y * toSeconds<float>(UpdateFrequency) * cosf(M_PI*k/64) / 20);
        }
    }

    shape.update(curTime);
}

bool Water::notifyScreenTransition(cpVect displacement)
{
    rect.l += displacement.x;
    rect.r += displacement.x;
    rect.t += displacement.y;
    rect.b += displacement.y;
    return true;
}

void Water::render(Renderer& renderer)
{
    renderer.pushTransform();
    renderer.currentTransform.translate(getDisplayPosition());
    renderer.pushDrawable(shape, {}, 21);
    renderer.popTransform();
}

