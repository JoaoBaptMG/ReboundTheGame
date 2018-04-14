#include "SavedGame.hpp"

#include <algorithm>
#include <memory>
#include <functional>
#include "misc/BitCounts.hpp"

#define CRYPT_OFF 0

#if !CRYPT_OFF
#include <cstring>
#include <random>
#include <numeric>
#include <chronoUtils.hpp>
#include <thread>
#include "streams/MemoryOutputStream.hpp"
#include "aes/tiny-AES-c-master/aes.hpp"
#endif

SavedGame::SavedGame() : levelInfo(0), goldenTokens{}, pickets{}, otherSecrets{}
{
    
}

size_t SavedGame::getGoldenTokenCount() const
{
    size_t count = 0;
    
    for (uint8_t k : goldenTokens)
        count += BitCounts[k];
    
    ASSERT(count <= 30);
    return count;
}

size_t SavedGame::getPicketCount() const
{
    size_t count = 0;

    for (uint8_t k : pickets)
        count += BitCounts[k];

    ASSERT(count <= 1000);
    return count;
}

size_t SavedGame::getPicketCountForLevel(size_t id) const
{
    ASSERT(id >= 1 && id <= 10);
    id--;
    size_t count = 0;

    size_t begin = 12*id + ((id+1)/2);

    for (size_t i = begin; i < begin+12; i++)
        count += BitCounts[pickets[i]];

    if (id % 2 == 0) count += BitCounts[pickets[begin+12] & 0x0F];
    else count += BitCounts[pickets[begin-1] & 0xF0];

    ASSERT(count <= 100);
    return count;
}

bool readBinaryVector(sf::InputStream& stream, std::vector<bool>& vector)
{
    size_t size;
    if (!readFromStream(stream, varLength(size))) return false;
    
    if (size == 0) return true;
    
    size_t memSize = (size + 7)/8;
    std::unique_ptr<uint8_t[]> vals{new uint8_t[memSize]};
    if (stream.read((char*)vals.get(), memSize*sizeof(uint8_t)) != memSize*sizeof(uint8_t))
        return false;
    
    size_t mask = 1;
    uint8_t* cur = vals.get();
    
    std::vector<bool> newVector(size);
    for (auto it = newVector.begin(); it != newVector.end(); ++it)
    {
        *it = (*cur & mask) != 0;
        mask <<= 1;
        if (mask == 256)
        {
            mask = 1;
            cur++;
        }
    }
    
    swap(vector, newVector);
    return true;
}

bool writeBinaryVector(OutputStream& stream, const std::vector<bool>& vector)
{
    if (!writeToStream(stream, varLength(vector.size()))) return false;
    if (vector.size() == 0) return true;
    
    size_t memSize = (vector.size() + 7) / 8;
    std::unique_ptr<uint8_t[]> vals{new uint8_t[memSize]};
    size_t mask = 1;
    uint8_t* cur = vals.get();
    
    for (auto it = vector.begin(); it != vector.end(); ++it)
    {
        if (mask == 1) *cur = 0;
        if (*it) *cur |= mask;
        mask <<= 1;
        if (mask == 256)
        {
            mask = 1;
            cur++;
        }
    }
    
    return stream.write((char*)vals.get(), memSize*sizeof(uint8_t));
}

#define ALL(v,e) std::all_of(std::begin(v), std::end(v), [&](auto&& x){ return e; })

bool readFromStream(sf::InputStream& stream, SavedGame& savedGame)
{
    return readFromStream(stream, savedGame.levelInfo, savedGame.goldenTokens, savedGame.pickets,
        savedGame.otherSecrets) && ALL(savedGame.mapsRevealed, readBinaryVector(stream, x));
}

bool writeToStream(OutputStream& stream, const SavedGame& savedGame)
{
    return writeToStream(stream, savedGame.levelInfo, savedGame.goldenTokens, savedGame.pickets,
        savedGame.otherSecrets) && ALL(savedGame.mapsRevealed, writeBinaryVector(stream, x));
}
#undef ALL

#if !CRYPT_OFF
uint64_t getCryptoRandomKey()
{
    uint64_t now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint64_t then = now;
    
    for (size_t i = 0; i < 64; i++)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(now % 2887));
        now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        then = now;
    }
    
    uint8_t mainKey[2*sizeof(uint64_t)];
    std::memcpy(mainKey, (const uint8_t*)&now, sizeof(uint64_t));
    std::memcpy(mainKey+sizeof(uint64_t), (const uint8_t*)&then, sizeof(uint64_t));
    
    AES_ctx ctx;
    AES_init_ctx(&ctx, mainKey);
    
    uint8_t secret[16] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    AES_CTR_xcrypt_buffer(&ctx, secret, 16);
    
    uint8_t index = 1 + secret[0] % 8;
    
    return *(uint64_t*)(secret+index);
}

constexpr size_t ScramblerSize = 7;
constexpr size_t EncryptRuns = 3;

std::vector<size_t> bitScramblerVector(uint64_t key, size_t size)
{
    std::vector<size_t> vec(8 * ScramblerSize * size);
    std::iota(vec.begin(), vec.end(), 0);
    std::shuffle(vec.begin(), vec.end(), std::mt19937_64(key));
    for (size_t& n : vec) n /= ScramblerSize;
    
    return vec;
}

void generateAESKey(uint64_t key, uint8_t* genKey)
{
    std::mt19937_64 gen(key);
    
    for (size_t i = 0; i < 4; i++)
    {
        uint64_t val = gen();
        std::memcpy(genKey+i*sizeof(uint64_t), (const uint8_t*)&val, sizeof(uint64_t));
    }
}
#endif

bool readEncryptedSaveFile(sf::InputStream& stream, SavedGame& savedGame, SavedGame::Key key)
{
#if CRYPT_OFF
    return readFromStream(stream, savedGame);
#endif
    
    std::vector<uint8_t> scrambledMem;
    if (!readFromStream(stream, scrambledMem)) return false;
    
    uint8_t aesKey[32];
    generateAESKey(key.aesGeneratorKey, aesKey);

    for (size_t i = 0; i < EncryptRuns; i++)
    {
        AES_ctx ctx;
        AES_init_ctx_iv(&ctx, aesKey, aesKey+16);
        AES_CBC_decrypt_buffer(&ctx, scrambledMem.data(), scrambledMem.size());
    }
    
    uint8_t padSize = scrambledMem.back();
    scrambledMem.resize(scrambledMem.size() - padSize);
    
    auto bitScrambler = bitScramblerVector(key.bitScramblingKey, scrambledMem.size()/ScramblerSize);
    std::vector<uint8_t> mem(scrambledMem.size()/ScramblerSize, 0);
    std::vector<uint8_t> alreadyRead(scrambledMem.size()/ScramblerSize, 0);
    
    for (size_t i = 0; i < bitScrambler.size(); i++)
    {
        size_t j = bitScrambler[i];
        bool bit = scrambledMem[i/8] & (1<<(i%8));
        
        if (alreadyRead[j/8] & (1<<(j%8)))
        {
            bool cmpBit = mem[j/8] & (1<<(j%8));
            if (bit != cmpBit)
                return false;
        }
        else
        {
            if (bit) mem[j/8] |= (1<<(j%8));
            else mem[j/8] &= ~(1<<(j%8));
            
            alreadyRead[j/8] |= (1<<(j%8));
        }
    }
    
    sf::MemoryInputStream mstream;
    mstream.open(mem.data(), mem.size());
    return readFromStream(mstream, savedGame);
}

bool writeEncryptedSaveFile(OutputStream& stream, const SavedGame& savedGame, SavedGame::Key& key)
{
    key = SavedGame::Key();
    
#if CRYPT_OFF
    return writeToStream(stream, savedGame);
#endif
    
    MemoryOutputStream mstream;
    if (!writeToStream(mstream, savedGame)) return false;
    
    const auto& mem = mstream.getContents();
    
    key.bitScramblingKey = getCryptoRandomKey();
    key.aesGeneratorKey = getCryptoRandomKey();
    
    auto bitScrambler = bitScramblerVector(key.bitScramblingKey, mem.size());
    std::vector<uint8_t> scrambledMem(ScramblerSize*mem.size());
    
    for (size_t i = 0; i < bitScrambler.size(); i++)
    {
        size_t j = bitScrambler[i];
        bool bit = mem[j/8] & (1<<(j%8));
        if (bit) scrambledMem[i/8] |= (1<<(i%8));
        else scrambledMem[i/8] &= ~(1<<(i%8));
    }
    
    size_t newSize = (scrambledMem.size() / 16 + 1) * 16;
    uint8_t diff = newSize - scrambledMem.size();
    scrambledMem.resize(newSize, diff);
    
    uint8_t aesKey[32];
    generateAESKey(key.aesGeneratorKey, aesKey);
    
    for (size_t i = 0; i < EncryptRuns; i++)
    {
        AES_ctx ctx;
        AES_init_ctx_iv(&ctx, aesKey, aesKey+16);
        AES_CBC_encrypt_buffer(&ctx, scrambledMem.data(), scrambledMem.size());
    }
    
    return writeToStream(stream, scrambledMem);
}
