#pragma once

#include <cstdint>
#include <vector>
#include <array>
#include <assert.hpp>
#include <SFML/System.hpp>
#include <OutputStream.hpp>
#include <VarLength.hpp>
#include <streamReaders.hpp>
#include <streamWriters.hpp>

struct SavedGame
{
    struct Key { uint64_t bitScramblingKey, aesGeneratorKey; };
    
    uint8_t levelInfo;
    uint8_t goldenTokens[4], pickets[125], otherSecrets[2];
    std::array<std::vector<bool>, 10> mapsRevealed;
    
    SavedGame();
    
    size_t getCurLevel() const { return levelInfo % 10 + 1; }
    size_t getAbilityLevel() const { return levelInfo / 10; }
    
    void setCurLevel(size_t l)
    {
        ASSERT(l >= 1 && l <= 10);
        levelInfo = (levelInfo / 10) * 10 + l - 1;
    }
    
    void setAbilityLevel(size_t l)
    {
        ASSERT(l >= 0 && l <= 10);
        levelInfo = l * 10 + levelInfo % 10;
    }
    
    bool getDoubleArmor() const { return otherSecrets[1] & 4; }
    void setDoubleArmor(bool da)
    {
        if (da) otherSecrets[1] |= 4;
        else otherSecrets[1] &= ~4;
    }

    bool getMoveRegen() const { return otherSecrets[1] & 8; }
    void setMoveRegen(bool mr)
    {
        if (mr) otherSecrets[1] |= 8;
        else otherSecrets[1] &= ~8;
    }
    
    bool getGoldenToken(size_t id) const
    {
        ASSERT(id < 30);
        return goldenTokens[id/8] & (1 << (id%8));
    }
    
    void setGoldenToken(size_t id, bool collected)
    {
        ASSERT(id < 30);
        if (collected) goldenTokens[id/8] |= (1 << (id%8));
        else goldenTokens[id/8] &= ~(1 << (id%8));
    }

    bool getPicket(size_t id) const
    {
        ASSERT(id < 1000);
        return pickets[id/8] & (1 << (id%8));
    }

    void setPicket(size_t id, bool collected)
    {
        ASSERT(id < 1000);
        if (collected) pickets[id/8] |= (1 << (id%8));
        else pickets[id/8] &= ~(1 << (id%8));
    }

    size_t getGoldenTokenCount() const;
    size_t getPicketCount() const;
    size_t getPicketCountForLevel(size_t id) const;

    bool getUPart(size_t id) const
    {
        ASSERT(id < 10);
        return otherSecrets[id/8] & (1 << (id%8));
    }
    
    void setUPart(size_t id, bool collected)
    {
        ASSERT(id < 10);
        if (collected) otherSecrets[id/8] |= (1 << (id%8));
        else otherSecrets[id/8] &= ~(1 << (id%8));
    }
};

bool readEncryptedSaveFile(sf::InputStream& stream, SavedGame& savedGame, SavedGame::Key key);
bool writeEncryptedSaveFile(OutputStream& stream, const SavedGame& savedGame, SavedGame::Key& key);
