#pragma once

#include "objects/Collectible.hpp"
#include "objects/Player.hpp"

#include "particles/ParticleBatch.hpp"

namespace collectibles
{
    class GoldenToken final : public ::Collectible
    {
        Sprite sprite;
        std::shared_ptr<cp::Shape> collisionShape;
        std::chrono::steady_clock::time_point initialTime;
        cpFloat baseY;
        ParticleBatch* tokenBatch;
        uint16_t tokenId;

    public:
        GoldenToken(GameScene& scene);
        virtual ~GoldenToken();

        void setupPhysics();

        virtual void onCollect(Player& player) override;

        virtual void update(std::chrono::steady_clock::time_point curTime) override;
        virtual void render(Renderer& renderer) override;
        
        virtual bool notifyScreenTransition(cpVect displacement) override;
    
#pragma pack(push, 1)
        struct ConfigStruct
        {
            sf::Vector2<int16_t> position;
            uint8_t tokenId;
        };

        static_assert(sizeof(ConfigStruct) == 5*sizeof(char), "Packing failed!");
#pragma pack(pop)

        bool configure(const ConfigStruct& config);
    };
}
