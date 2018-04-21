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


#pragma once

#include <portaudio.h>
#include <bitset>
#include <array>
#include <unordered_map>
#include <memory>
#include <non_copyable_movable.hpp>
#include "AudioInstance.hpp"
#include "AudioCommand.hpp"
#include "readerwriterqueue/readerwriterqueue.h"

constexpr size_t MaxSounds = 64;

class AudioManager final : public util::non_copyable
{
    moodycamel::ReaderWriterQueue<AudioCommand> commandQueue;
    moodycamel::ReaderWriterQueue<AudioReference> audioStopQueue;

    std::bitset<MaxSounds> instancesUsed;
    std::array<AudioInstance, MaxSounds> audioInstancesHostSide;
    std::array<AudioInstanceLight, MaxSounds> audioInstancesThreadSide;

    PaStream* currentStream;
    AudioReference findEmptyInstance();
    int audioFunction(int32_t* out, size_t numFrames);

public:
    AudioManager();
    ~AudioManager();

    void update();

    AudioReference playSound(std::shared_ptr<Sound> sound, float volume = 1.0f, float logPitch = 0.0f, float balance = 0.0f);
    void setSoundVolume(const AudioReference& ref, float volume);
    void setSoundLogPitch(const AudioReference& ref, float logPitch);
    void setSoundBalance(const AudioReference& ref, float balance);
    void stopSound(const AudioReference& ref);
};
