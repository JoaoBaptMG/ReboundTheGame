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
#include <glm/glm.hpp>

#include "../Texture.hpp"

class GlTexture
{
	static GLint prevTextureBinding[4];

	GLuint id;
	bool useMipmaps;
	TextureFormat format;

	void raiseEmptyError();

public:
	GlTexture(size_t width, size_t height, TextureFormat format = TextureFormat::UInt8N4);
	~GlTexture();

	void setSamplerParameters(TextureFilter magFilter, TextureFilter minFilter, bool enableMipmaps,
		TextureFilter mipFilter, TextureEdge edgeX, TextureEdge edgeY);

	void uploadData(const void* data, size_t size);
	void bind(size_t unit);
};