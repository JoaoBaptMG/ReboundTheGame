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


#include "WindowHandler.hpp"
#include "defaults.hpp"
#include <stdexcept>
#include <glad/glad.h>
#include <rect.hpp>

WindowHandler::WindowHandler(bool fullscreen, bool vsync) : renderWindow(nullptr), vsyncEnabled(vsync), fullscreen(fullscreen),
	prevx(0), prevy(0)
{
	if (!glfwInit()) throw std::runtime_error("Could not initialize GLFW properly!");

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 2);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	GLFWmonitor* monitor;
	int width, height;
	if (fullscreen)
	{
		monitor = glfwGetPrimaryMonitor();
		auto videoMode = glfwGetVideoMode(monitor);
		width = videoMode->width;
		height = videoMode->height;

		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
	}
	else
	{
		monitor = nullptr;
		width = ScreenWidth;
		height = ScreenHeight;
	}

	renderWindow = glfwCreateWindow(width, height, "Game", monitor, nullptr);
	if (!renderWindow) throw std::runtime_error("Could not create the GLFW window!");

	glfwSetInputMode(renderWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSwapInterval(vsyncEnabled);

	setWindowViewport();
}

WindowHandler::~WindowHandler()
{
	if (renderWindow) glfwDestroyWindow(renderWindow);
	glfwTerminate();
}

void WindowHandler::setFullscreen(bool fullscreen)
{
    if (getFullscreen() == fullscreen) return;
    
    if (fullscreen) enableFullscreen();
    else disableFullscreen();
    
	glfwSwapInterval(vsyncEnabled);
	setWindowViewport();
}

void WindowHandler::enableFullscreen()
{
	fullscreen = true;

	int sz;
	auto monitors = glfwGetMonitors(&sz);

	int x, y;
	glfwGetWindowPos(renderWindow, &x, &y);
	prevx = x; prevy = y;
	util::irect windowRect(x, y, ScreenWidth, ScreenHeight);

	int mi = 0, marea = 0;
	for (int i = 0; i < sz; i++)
	{
		auto videoMode = glfwGetVideoMode(monitors[i]);

		int vx, vy;
		glfwGetMonitorPos(monitors[i], &vx, &vy);

		util::irect monitorRect(vx, vy, videoMode->width, videoMode->height);
		if (!windowRect.intersects(monitorRect)) continue;

		auto intersection = windowRect.intersect(monitorRect);
		int area = intersection.width * intersection.height;

		if (marea < area) mi = i, marea = area;
	}

	auto videoMode = glfwGetVideoMode(monitors[mi]);

	int vx, vy;
	glfwGetMonitorPos(monitors[mi], &vx, &vy);

	glfwSetWindowMonitor(renderWindow, monitors[mi], vx, vy, videoMode->width, videoMode->height, videoMode->refreshRate);
}

void WindowHandler::disableFullscreen()
{
	fullscreen = false;
	glfwSetWindowMonitor(renderWindow, nullptr, prevx, prevy, ScreenWidth, ScreenHeight, 0);
}

void WindowHandler::setWindowViewport()
{
	int width, height;
	glfwGetWindowSize(renderWindow, &width, &height);

	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);

	int64_t ratio = (int64_t)width * ScreenHeight - (int64_t)height * ScreenWidth;
	if (ratio > 0)
	{
		int newWidth = (int64_t)height * ScreenWidth / ScreenHeight;
		int ox = (width - newWidth) / 2;
		glViewport(ox, 0, newWidth, height);
	}
	else if (ratio < 0)
	{
		int newHeight = (int64_t)width * ScreenHeight / ScreenWidth;
		int oy = (height - newHeight) / 2;
		glViewport(0, oy, width, newHeight);
	}
}

void WindowHandler::prepareForDraw()
{
    renderer.clearState();
}

void WindowHandler::display()
{
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
	renderer.render();
	glfwSwapBuffers(renderWindow);
}
