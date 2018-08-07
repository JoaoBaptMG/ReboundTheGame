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

#include <SFML/Graphics.hpp>
#include <chronoUtils.hpp>
#include <memory>
#include <mutex>
#include <glm/glm.hpp>

class WaterBody final : public sf::Drawable
{
	static sf::Shader& getDrawingShader();

#pragma pack(push, 1)
	struct StaticWaveProperties
	{
		float ofs, amp, k, omega;
		operator sf::Glsl::Vec4()
		{
			return sf::Glsl::Vec4{ ofs, amp, k, omega };
		}
	};
#pragma pack(pop)

	StaticWaveProperties generatedWaves[8];

	struct DynamicWaveProperties
	{
		size_t width;
		std::vector<float> previousFrame2, previousFrame, curFrame, newVelocity;
		std::vector<sf::Uint8> texBuffer;
		sf::Texture texture;
		std::mutex updateMutex, velocityMutex;
	};

	glm::vec2 drawingSize;
	FrameTime startingTime, curTime;
	sf::Vertex quad[4];
	sf::Color color, coastColor;
	DynamicWaveProperties* dynamicWaveProperties;

	intmax_t curT;
	mutable bool haltSimulation;
	bool topHidden;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
	WaterBody(glm::vec2 drawingSize);
	~WaterBody();

	void recreateQuad();
	void resetWaves();
	void resetSimulationVectors();
	void update(FrameTime curTime);
	void updateSimulation();

	void setVelocity(float point, float newVel);

	auto getDrawingSize() const { return drawingSize; }
	void setDrawingSize(glm::vec2 size) { drawingSize = size; recreateQuad(); resetSimulationVectors(); }

	auto getColor() const { return color; }
	void setColor(sf::Color color) { this->color = color; }

	auto getCoastColor() const { return coastColor; }
	void setCoastColor(sf::Color color) { coastColor = color; }

	auto getTopHidden() const { return topHidden; }
	void setTopHidden(bool top) { topHidden = top; recreateQuad(); resetWaves(); resetSimulationVectors(); }

	friend struct DynamicUpdateThreadInfo;
	friend void dynamicWaveUpdateThread(void* ptr);
};
