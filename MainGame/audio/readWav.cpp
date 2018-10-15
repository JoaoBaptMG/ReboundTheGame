//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "readWav.hpp"

#include <streamReaders.hpp>
#include <limits>
#include "Sound.hpp"

util::generic_shared_ptr loadWaveFile(std::unique_ptr<sf::InputStream>& stream)
{
    using namespace util;

    uint32_t dummy, chunkSize;
    if (!checkMagic(*stream, "RIFF") || !readFromStream(*stream, dummy) ||
        !checkMagic(*stream, "WAVEfmt ") || !readFromStream(*stream, chunkSize)) return generic_shared_ptr{};
    if (chunkSize != 16) return generic_shared_ptr{};

    uint16_t audioFormat, numChannels, bitsPerSample, dummy16;
    uint32_t sampleRate;
    if (!readFromStream(*stream, audioFormat, numChannels, sampleRate, dummy, dummy16, bitsPerSample)) return generic_shared_ptr{};
    if (audioFormat != 1 || numChannels != 1) return generic_shared_ptr{};

    uint32_t totalSoundSize;
    if (!checkMagic(*stream, "data") || !readFromStream(*stream, totalSoundSize)) return generic_shared_ptr{};

    auto snd = std::make_shared<Sound>();
    snd->stereo = false;
    snd->sampleRate = sampleRate;
    snd->loopPoint = std::numeric_limits<size_t>::max();
    snd->data.resize(totalSoundSize * 8 / bitsPerSample);

    for (float& val : snd->data)
    {
        if (bitsPerSample == 8)
        {
            uint8_t sample;
            if (!readFromStream(*stream, sample)) return generic_shared_ptr{};
            val = (sample - 128) / 128.0f;
        }
        else
        {
            int16_t sample;
            if (!readFromStream(*stream, sample)) return generic_shared_ptr{};
            val = sample / 32768.0f;
        }
    }

    return generic_shared_ptr{snd};
}
