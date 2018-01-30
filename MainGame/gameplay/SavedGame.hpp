#pragma once

#include <cstdint>
#include <vector>
#include <assert.hpp>
#include <SFML/System.hpp>
#include <OutputStream.hpp>
#include <VarLength.hpp>
#include <streamReaders.hpp>
#include <streamWriters.hpp>

struct SavedGame
{
    struct Key { uint64_t bitScramblingKey, aesGeneratorKey; };
    
    uint8_t curLevel, abilityLevel;
    uint8_t secretItems[5];
    std::vector<bool> mapsRevealed[10];
    
    SavedGame() : curLevel(1), abilityLevel(0), secretItems{0, 0, 0, 0, 0}, mapsRevealed{} {}
    
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
