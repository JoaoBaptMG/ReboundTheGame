#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <chrono>

class LevelData;

class GUIMap final : public sf::Drawable
{
    std::shared_ptr<LevelData> curLevel;
    sf::Texture* mapTexture;
    sf::VertexArray vertArray;
    sf::Vector2f displayPosition;
    size_t curRoom;

    std::chrono::steady_clock::time_point initTime;
    bool extendedFrame;
    
public:
    GUIMap(bool extendedFrame = false) : extendedFrame(extendedFrame) {}
    ~GUIMap() {}
    
    void update(std::chrono::steady_clock::time_point curTime);
    
    auto getCurLevel() const { return curLevel; }
    void setCurLevel(std::shared_ptr<LevelData> level);
    void buildLevelTexture();
    
    auto getCurRoom() const { return curRoom; }
    void setCurRoom(size_t room) { curRoom = room; }
    
    auto getDisplayPosition() const { return displayPosition; }
    void setDisplayPosition(sf::Vector2f pos) { displayPosition = pos; }
    
    auto getExtendedFrame() const { return extendedFrame; }
    void setExtendedFrame(bool f) { extendedFrame = f; }
    
    sf::FloatRect getBounds() const;
    
    void presentRoom(size_t room);
    void presentRoomFull(size_t room);
    void hideRoom(size_t room);
    
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
