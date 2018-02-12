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
    
    uint8_t levelInfoDoubleArmor;
    uint8_t secretItems[5];
    std::array<std::vector<bool>, 10> mapsRevealed;
    
    SavedGame();
    
    size_t getCurLevel() const { return levelInfoDoubleArmor % 10 + 1; }
    size_t getAbilityLevel() const { return levelInfoDoubleArmor / 10; }
    
    void setCurLevel(size_t l)
    {
        ASSERT(l >= 1 && l <= 10);
        levelInfoDoubleArmor = (levelInfoDoubleArmor / 10) * 10 + l - 1;
    }
    
    void setAbilityLevel(size_t l)
    {
        ASSERT(l >= 0 && l <= 10);
        levelInfoDoubleArmor = l * 10 + levelInfoDoubleArmor % 10;
    }
    
    bool getDoubleArmor() const { return levelInfoDoubleArmor & 128; }
    
    void setDoubleArmor(bool da)
    {
        if (da) levelInfoDoubleArmor |= 128;
        else levelInfoDoubleArmor &= ~128;
    }
    
    bool getGoldenToken(size_t id) const
    {
        ASSERT(id < 30);
        return secretItems[id/8] & (1 << (id%8));
    }
    
    void setGoldenToken(size_t id, bool collected)
    {
        ASSERT(id < 30);
        if (collected) secretItems[id/8] |= (1 << (id%8));
        else secretItems[id/8] &= ~(1 << (id%8));
    }
    
    size_t getGoldenTokenCount() const;
    
    bool getUPart(size_t id) const
    {
        ASSERT(id < 10);
        size_t realId = 30 + id;
        return secretItems[realId/8] & (1 << (realId%8));
    }
    
    void setUPart(size_t id, bool collected)
    {
        ASSERT(id < 10);
        size_t realId = 30 + id;
        if (collected) secretItems[realId/8] |= (1 << (realId%8));
        else secretItems[realId/8] &= ~(1 << (realId%8));
    }
};

bool readEncryptedSaveFile(sf::InputStream& stream, SavedGame& savedGame, SavedGame::Key key);
bool writeEncryptedSaveFile(OutputStream& stream, const SavedGame& savedGame, SavedGame::Key& key);
