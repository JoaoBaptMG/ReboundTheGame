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


#include <iostream>
#include <chronoUtils.hpp>
#include <thread>
#include <utility>
#include <clocale>
#include <SFML/Graphics.hpp>

#include <grid.hpp>
#include <assert.hpp>
#include "input/InputManager.hpp"
#include "input/InputPlayerController.hpp"
#include "scene/Scene.hpp"
#include "resources/ResourceManager.hpp"
#include "resources/FilesystemResourceLocator.hpp"
#include "drawables/Tilemap.hpp"
#include "scene/SceneManager.hpp"
#include "scene/GameScene.hpp"
#include "objects/Player.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/WindowHandler.hpp"
#include "settings/Settings.hpp"
#include "language/LocalizationManager.hpp"
#include "language/LanguageDescriptor.hpp"
#include "language/KeyboardKeyName.hpp"
#include "audio/AudioManager.hpp"
#include "Services.hpp"

#include "scene/TitleScene.hpp"

#include <chronoUtils.hpp>

#define DEBUG_STEADY 0

using namespace std::literals::chrono_literals;

bool clearMapTextures();
bool GlobalUpdateWindowHandler;
int main(int argc, char **argv)
{
    auto locale = std::setlocale(LC_ALL, "");

    bool success;
    auto settings = loadSettingsFile(&success);
    if (!success)
    {
        std::cout << "WARNING! Settings file not loaded properly!" << std::endl;
        settings.languageFile = languageDescriptorForLocale(locale);
    }
    
    GlobalUpdateWindowHandler = false;
    WindowHandler windowHandler(settings.videoSettings.fullscreen, settings.videoSettings.vsyncEnabled);

    InputManager inputManager;

    ResourceManager resourceManager;
    resourceManager.setResourceLocator(new FilesystemResourceLocator());
    
    LocalizationManager localizationManager(true);
    localizationManager.loadLanguageDescriptor(settings.languageFile);

    AudioManager audioManager;

    Services services { audioManager, inputManager, localizationManager, resourceManager, settings };

    auto scene = new TitleScene(services);
    
    auto updateTime = FrameClock::now();
    
    SceneManager sceneManager;
    sceneManager.pushScene(scene);

    while (windowHandler.getWindow().isOpen())
    {
        sf::Event event;

        while (windowHandler.getWindow().pollEvent(event))
        {
            if (inputManager.handleEvent(event)) continue;
            switch (event.type)
            {
                case sf::Event::EventType::Closed:
                    windowHandler.getWindow().close();
                    break;
                default: break;
            }
        }

        auto curTime = FrameClock::now();

        while (curTime <= updateTime)
        {
            std::this_thread::sleep_until(updateTime);
            curTime = FrameClock::now();
        }

        while (curTime > updateTime)
        {
            updateTime += UpdatePeriod;
            sceneManager.update(updateTime);
#if DEBUG_STEADY
            break;
#endif
        }
        
        if (!sceneManager.hasScenes())
        {
            windowHandler.getWindow().close();
            break;
        }

        if (GlobalUpdateWindowHandler)
        {
            windowHandler.setFullscreen(settings.videoSettings.fullscreen);
            windowHandler.setVsyncEnabled(settings.videoSettings.vsyncEnabled);
            GlobalUpdateWindowHandler = false;
        }

        windowHandler.prepareForDraw();
        sceneManager.render(windowHandler.getRenderer());
        windowHandler.display();

        audioManager.update();
    }

    clearMapTextures();

    if (!storeSettingsFile(settings))
        std::cout << "WARNING! Settings file not stored properly!" << std::endl;

    return 0;
}
