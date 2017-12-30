#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <chrono>

struct LevelData;

class GUIMap final : public sf::Drawable
{
    std::shared_ptr<LevelData> curLevel;
    sf::Texture curTexture;
    sf::VertexArray vertArray;
    sf::Vector2f displayPosition;
    size_t curRoom;

    std::chrono::steady_clock::time_point initTime;
    
public:
    GUIMap() : vertArray(sf::PrimitiveType::Triangles) {}
    ~GUIMap() {}
    
    void update(std::chrono::steady_clock::time_point curTime);
    
    auto getCurLevel() const { return curLevel; }
    void setCurLevel(std::shared_ptr<LevelData> level);
    void buildLevelTexture();
    
    auto getCurRoom() const { return curRoom; }
    void setCurRoom(size_t room) { curRoom = room; }
    
    auto getDisplayPosition() const { return displayPosition; }
    void setDisplayPosition(sf::Vector2f pos) { displayPosition = pos; }
    
    void presentRoom(size_t room);
    void presentRoomFull(size_t room);
    
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
