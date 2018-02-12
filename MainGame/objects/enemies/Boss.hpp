#pragma once

#include "EnemyCommon.hpp"
#include "objects/GUI.hpp"
#include "drawables/TextDrawable.hpp"

#include "language/LangID.hpp"
#include <chrono>

class GameScene;
class Renderer;

namespace enemies
{
    class Boss : public EnemyCommon, public GUIBossUpdater
    {
    protected:
        Boss(GameScene& scene, LangID presenterID);
        
    public:
        virtual ~Boss();
        
        virtual size_t getCurrentHealth() const { return getHealth(); }
    };
    
    class BossCaption : public ::GameObject
    {
        TextDrawable text;
        std::chrono::steady_clock::time_point curTime, initTime;
        
    public:
        BossCaption(GameScene& scene, LangID captionID);
        virtual ~BossCaption() {}
        
        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
    };
}
