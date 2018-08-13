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

#include "glad/glad.h"
#include <rect.hpp>
#include <glm/glm.hpp>
#include <non_copyable_movable.hpp>
#include <vector>

#include "../Texture.hpp"

class GlTexture : util::non_copyable_movable
{
	static std::vector<GLint> prevTextureBinding;
	static GLint prevActiveTexture;
	static void initBindings();

	GLuint id;
	bool useMipmaps;
	TextureFormat format;
	size_t width, height;

	void raiseEmptyError();

public:
	GlTexture(size_t width, size_t height, TextureFormat format = TextureFormat::UInt8N4);
	~GlTexture();

	void setSamplerParameters(TextureFilter magFilter, TextureFilter minFilter, TextureEdge edgeX, TextureEdge edgeY);
	void setSamplerParameters(TextureFilter magFilter, TextureFilter minFilter, TextureFilter mipFilter, TextureEdge edgeX, TextureEdge edgeY);

	void uploadData(const void* data);
	void uploadSubData(const void* data, util::irect rect);
	void bind(size_t unit);

	glm::uvec2 getSize() const { return glm::uvec2(width, height); }
};