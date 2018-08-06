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


#include "AudioManager.hpp"
#include "AudioException.hpp"
#include <portaudio.h>
#include <algorithm>
#include <predUtils.hpp>
#include <cmath>

constexpr double CanonicalSampleRate = 44100;
constexpr size_t BufferSize = CanonicalSampleRate / 300;
constexpr uint32_t Scale = (int32_t)1 << 30;

inline void checkAndThrow(PaError error)
{
    if (error) throw AudioException(error);
}

AudioManager::AudioManager() : commandQueue(64), audioStopQueue(64), samplesPassed(0)
{
    checkAndThrow(Pa_Initialize());
    checkAndThrow(Pa_OpenDefaultStream(&currentStream, 0, 2, paInt32, CanonicalSampleRate, paFramesPerBufferUnspecified,
    [](const void* in, void* out, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
        void* userData) { return ((AudioManager*)userData)->audioFunction((int32_t*)out, frameCount); }, (void*)this));
    checkAndThrow(Pa_StartStream(currentStream));
}

AudioManager::~AudioManager()
{
    checkAndThrow(Pa_AbortStream(currentStream));
    checkAndThrow(Pa_CloseStream(currentStream));
    checkAndThrow(Pa_Terminate());
}

AudioReference AudioManager::findEmptyInstance()
{
    for (size_t i = 0; i < MaxSounds; i++)
        if (!instancesUsed[i]) return i;

    return -1;
}

AudioReference AudioManager::playSound(std::shared_ptr<Sound> sound, float volume, float logPitch, float balance)
{
    auto ref = findEmptyInstance();

    if (ref != -1)
    {
        {
            AudioCommand cmd;
            cmd.type = AudioCommand::Type::Play1;
            cmd.ref = ref;
            cmd.play1.sound = sound.get();
            cmd.play1.volume = volume;
            commandQueue.enqueue(cmd);
        }

        {
            AudioCommand cmd;
            cmd.type = AudioCommand::Type::Play2;
            cmd.ref = ref;
            cmd.play2.logPitch = logPitch;
            cmd.play2.balance = balance/2;
            commandQueue.enqueue(cmd);
        }

        audioInstancesHostSide[ref].sound = sound;
        audioInstancesHostSide[ref].volume = volume;
        audioInstancesHostSide[ref].logPitch = logPitch;
        audioInstancesHostSide[ref].balance = balance;

        instancesUsed[ref] = true;
    }
    
    return ref;
}

void AudioManager::setSoundVolume(const AudioReference& ref, float volume)
{
    AudioCommand cmd;
    cmd.type = AudioCommand::Type::Update;
    cmd.ref = ref;
    cmd.update.data = AudioCommand::Volume;
    cmd.update.newVal = volume;
    commandQueue.enqueue(std::move(cmd));

    audioInstancesHostSide[ref].volume = volume;
}

void AudioManager::setSoundLogPitch(const AudioReference& ref, float logPitch)
{
    AudioCommand cmd;
    cmd.type = AudioCommand::Type::Update;
    cmd.ref = ref;
    cmd.update.data = AudioCommand::LogPitch;
    cmd.update.newVal = logPitch;
    commandQueue.enqueue(std::move(cmd));

    audioInstancesHostSide[ref].logPitch = logPitch;
}

void AudioManager::setSoundBalance(const AudioReference& ref, float balance)
{
    AudioCommand cmd;
    cmd.type = AudioCommand::Type::Update;
    cmd.ref = ref;
    cmd.update.data = AudioCommand::Balance;
    cmd.update.newVal = balance/2;
    commandQueue.enqueue(std::move(cmd));

    audioInstancesHostSide[ref].balance = balance;
}

void AudioManager::stopSound(const AudioReference& ref)
{
    AudioCommand cmd;
    cmd.type = AudioCommand::Type::Stop;
    cmd.ref = ref;
    commandQueue.enqueue(std::move(cmd));
}

void AudioManager::fadeSound(const AudioReference& ref, FrameDuration dur, float toVol)
{
    size_t numSamples = toSeconds<size_t>(CanonicalSampleRate * dur);
    float fadeOfs = (toVol - audioInstancesHostSide[ref].volume) / numSamples;

    AudioCommand cmd;
    cmd.type = AudioCommand::Type::Fade;
    cmd.ref = ref;
    cmd.fade.fadeCount = numSamples;
    cmd.fade.fadeOfs = fadeOfs;
    commandQueue.enqueue(std::move(cmd));

    audioInstancesHostSide[ref].fadeCount = numSamples;
    audioInstancesHostSide[ref].fadeOfs = fadeOfs;
}

inline void updateSampleIncr(AudioInstanceLight& instance)
{
	if (instance.sound)
		instance.sampleIncr = 65536ULL * exp2f(instance.logPitch) * instance.sound->sampleRate / CanonicalSampleRate;
}

int AudioManager::audioFunction(int32_t* out, size_t numFrames)
{
    AudioCommand cmd;
    size_t j = 0;
    while (commandQueue.try_dequeue(cmd) && j < 4)
    {
        switch (cmd.type)
        {
            case AudioCommand::Type::Play1:
                audioInstancesThreadSide[cmd.ref].sound = cmd.play1.sound;
                audioInstancesThreadSide[cmd.ref].curSample = 0;
                audioInstancesThreadSide[cmd.ref].volume = cmd.play1.volume;
                break;
            case AudioCommand::Type::Play2:
                audioInstancesThreadSide[cmd.ref].curSampleFractional = 0;
                audioInstancesThreadSide[cmd.ref].logPitch = cmd.play2.logPitch;
                audioInstancesThreadSide[cmd.ref].balance = cmd.play2.balance;
                updateSampleIncr(audioInstancesThreadSide[cmd.ref]);
                break;
            case AudioCommand::Type::Update:
				if (!audioInstancesThreadSide[cmd.ref].sound) break;
                switch (cmd.update.data)
                {
                    case AudioCommand::Volume: audioInstancesThreadSide[cmd.ref].volume = cmd.update.newVal; break;
                    case AudioCommand::LogPitch:
                        audioInstancesThreadSide[cmd.ref].logPitch = cmd.update.newVal;
                        updateSampleIncr(audioInstancesThreadSide[cmd.ref]);
                        break;
                    case AudioCommand::Balance: audioInstancesThreadSide[cmd.ref].balance = cmd.update.newVal; break;
				} break;
            case AudioCommand::Type::Fade:
				if (!audioInstancesThreadSide[cmd.ref].sound) break;
                audioInstancesThreadSide[cmd.ref].fadeCount = cmd.fade.fadeCount;
                audioInstancesThreadSide[cmd.ref].fadeOfs = cmd.fade.fadeOfs;
                break;
            case AudioCommand::Type::Stop:
				if (!audioInstancesThreadSide[cmd.ref].sound) break;
                audioInstancesThreadSide[cmd.ref].sound = nullptr;
                audioStopQueue.try_enqueue(cmd.ref);
                break;
        }

        j++;
    }

    samplesPassed += numFrames;
    std::fill_n(out, 2*numFrames, 0);

    j = 0;
    for (auto& instance : audioInstancesThreadSide)
    {
        if (!instance.sound) continue;

        for (size_t i = 0; i < numFrames; i++)
        {
            if (instance.sound->stereo)
            {
                out[2*i] += Scale * instance.sound->data[2*instance.curSample] * instance.volume * (0.5 - instance.balance);
                out[2*i+1] += Scale * instance.sound->data[2*instance.curSample+1] * instance.volume * (0.5 + instance.balance);
            }
            else
            {
                out[2*i] += Scale * instance.sound->data[instance.curSample] * instance.volume * (0.5 - instance.balance);
                out[2*i+1] += Scale * instance.sound->data[instance.curSample] * instance.volume * (0.5 + instance.balance);
            }
 
            instance.curSampleFractional += instance.sampleIncr;
            instance.curSample += instance.curSampleFractional >> 16;
            instance.curSampleFractional &= 65535ULL;
            if (instance.curSample >= instance.sound->data.size()) break;

            if (instance.fadeCount > 0)
            {
                instance.volume += instance.fadeOfs;
                instance.fadeCount--;

                if (instance.fadeCount == 0 && instance.volume <= 0)
                {
                    instance.sound = nullptr;
                    audioStopQueue.try_enqueue(j);
                    break;
                }
            }
        }

        if (instance.curSample >= instance.sound->size())
        {
            if (instance.sound->loopPoint != std::numeric_limits<size_t>::max())
                instance.curSample -= instance.sound->size() - instance.sound->loopPoint;
            else
            {
                instance.sound = nullptr;
                audioStopQueue.try_enqueue(j);
            }
        }

        j++;
    }

    return paContinue;
}

void AudioManager::update()
{
    AudioReference audioToStop;

    for (auto& instance : audioInstancesHostSide)
    {
        if (instance.fadeCount > 0)
        {
            size_t fadeMax = std::min<size_t>(instance.fadeCount, samplesPassed);

            instance.volume += fadeMax * instance.fadeOfs;
            instance.fadeCount -= fadeMax;
        }
    }

    samplesPassed = 0;

    while (audioStopQueue.try_dequeue(audioToStop))
    {
        audioInstancesHostSide[audioToStop].sound = nullptr;
        instancesUsed[audioToStop] = false;
    }
}
