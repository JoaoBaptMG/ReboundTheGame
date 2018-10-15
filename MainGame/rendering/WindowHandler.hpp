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

#include "Renderer.hpp"
#include <GLFW/glfw3.h>
#include <non_copyable_movable.hpp>

class WindowHandler : util::non_copyable_movable
{
	GLFWwindow* renderWindow;
    Renderer renderer;
	int prevx, prevy;
    
    bool vsyncEnabled;
	bool fullscreen;
    
    void enableFullscreen();
    void disableFullscreen();
	void setWindowViewport();
    
public:
    WindowHandler(bool fullscreen, bool vsync);
	~WindowHandler();
    
    void setFullscreen(bool fullscreen);
	auto getFullscreen() const { return fullscreen; }
    
    void setVsyncEnabled(bool vsync) { vsyncEnabled = vsync; glfwSwapInterval(vsync); }
    
    Renderer& getRenderer() { return renderer; }
    GLFWwindow* getWindow() { return renderWindow; }
    
    void prepareForDraw();
    void display();
};
