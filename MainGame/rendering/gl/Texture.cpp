//
// Copyright (c) 2016-2018 João Baptista de Paula e Silva.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following TextureFormat::conditions:
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


#include "Texture.hpp"

std::vector<GLint> GlTexture::prevTextureBinding{};
GLint GlTexture::prevActiveTexture = -1;

void GlTexture::initBindings()
{
	static bool inited = false;

	if (!inited)
	{
		inited = true;

		GLint numTextures;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numTextures);
		prevTextureBinding.resize(numTextures, -1);
	}
}

void GlTexture::raiseEmptyError()
{

}

GlTexture::~GlTexture()
{
	glDeleteTextures(1, &id);
}

static GLint glFromTextureFilter(TextureFilter f)
{
	switch (f)
	{
	case TextureFilter::Nearest: return GL_NEAREST;
	case TextureFilter::Linear: return GL_LINEAR;
	}
}

static GLint glFromTextureFilter(TextureFilter f, TextureFilter fm)
{
	if (f == TextureFilter::Nearest && fm == TextureFilter::Nearest)
		return GL_NEAREST_MIPMAP_NEAREST;
	if (f == TextureFilter::Nearest && fm == TextureFilter::Linear)
		return GL_NEAREST_MIPMAP_LINEAR;
	if (f == TextureFilter::Linear && fm == TextureFilter::Nearest)
		return GL_LINEAR_MIPMAP_NEAREST;
	if (f == TextureFilter::Linear && fm == TextureFilter::Linear)
		return GL_LINEAR_MIPMAP_LINEAR;
}

static GLint glFromTextureEdge(TextureEdge e)
{
	switch (e)
	{
	case TextureEdge::Wrap: return GL_REPEAT;
	case TextureEdge::Mirror: return GL_MIRRORED_REPEAT;
	case TextureEdge::Clamp: return GL_CLAMP_TO_EDGE;
	}
}

struct GlFormat { GLint internalFormat; GLenum format, type; };

GlFormat glFromFormat(TextureFormat f)
{
	switch (f)
	{
	case TextureFormat::UInt8N1: return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt8N2: return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt8N3: return { GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt8N4: return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt16N1: return { GL_R16, GL_RED, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt16N2: return { GL_RG16, GL_RG, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt16N3: return { GL_RGB16, GL_RGB, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt16N4: return { GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT };
	case TextureFormat::Int8N1: return { GL_R8_SNORM, GL_RED, GL_BYTE };
	case TextureFormat::Int8N2: return { GL_RG8_SNORM, GL_RG, GL_BYTE };
	case TextureFormat::Int8N3: return { GL_RGB8_SNORM, GL_RGB, GL_BYTE };
	case TextureFormat::Int8N4: return { GL_RGBA8_SNORM, GL_RGBA, GL_BYTE };
	case TextureFormat::Int16N1: return { GL_R16_SNORM, GL_RED, GL_SHORT };
	case TextureFormat::Int16N2: return { GL_RG16_SNORM, GL_RG, GL_SHORT };
	case TextureFormat::Int16N3: return { GL_RGB16_SNORM, GL_RGB, GL_SHORT };
	case TextureFormat::Int16N4: return { GL_RGBA16_SNORM, GL_RGBA, GL_SHORT };
	case TextureFormat::UInt8I1: return { GL_R8UI, GL_RED, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt8I2: return { GL_RG8UI, GL_RG, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt8I3: return { GL_RGB8UI, GL_RGB, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt8I4: return { GL_RGBA8UI, GL_RGBA, GL_UNSIGNED_BYTE };
	case TextureFormat::UInt16I1: return { GL_R16UI, GL_RED, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt16I2: return { GL_RG16UI, GL_RG, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt16I3: return { GL_RGB16UI, GL_RGB, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt16I4: return { GL_RGBA16UI, GL_RGBA, GL_UNSIGNED_SHORT };
	case TextureFormat::UInt32I1: return { GL_R32UI, GL_RED, GL_UNSIGNED_INT };
	case TextureFormat::UInt32I2: return { GL_RG32UI, GL_RG, GL_UNSIGNED_INT };
	case TextureFormat::UInt32I3: return { GL_RGB32UI, GL_RGB, GL_UNSIGNED_INT };
	case TextureFormat::UInt32I4: return { GL_RGBA32UI, GL_RGBA, GL_UNSIGNED_INT };
	case TextureFormat::Int8I1: return { GL_R8I, GL_RED, GL_BYTE };
	case TextureFormat::Int8I2: return { GL_RG8I, GL_RG, GL_BYTE };
	case TextureFormat::Int8I3: return { GL_RGB8I, GL_RGB, GL_BYTE };
	case TextureFormat::Int8I4: return { GL_RGBA8I, GL_RGBA, GL_BYTE };
	case TextureFormat::Int16I1: return { GL_R16I, GL_RED, GL_SHORT };
	case TextureFormat::Int16I2: return { GL_RG16I, GL_RG, GL_SHORT };
	case TextureFormat::Int16I3: return { GL_RGB16I, GL_RGB, GL_SHORT };
	case TextureFormat::Int16I4: return { GL_RGBA16I, GL_RGBA, GL_SHORT };
	case TextureFormat::Int32I1: return { GL_R32I, GL_RED, GL_INT };
	case TextureFormat::Int32I2: return { GL_RG32I, GL_RG, GL_INT };
	case TextureFormat::Int32I3: return { GL_RGB32I, GL_RGB, GL_INT };
	case TextureFormat::Int32I4: return { GL_RGBA32I, GL_RGBA, GL_INT };
	case TextureFormat::Float1: return { GL_R32F, GL_RED, GL_FLOAT };
	case TextureFormat::Float2: return { GL_RG32F, GL_RG, GL_FLOAT };
	case TextureFormat::Float3: return { GL_RGB32F, GL_RGB, GL_FLOAT };
	case TextureFormat::Float4: return { GL_RGBA32F, GL_RGBA, GL_FLOAT };
	case TextureFormat::RGB10A2: return { GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_10_10_10_2 };
	default: return { 0, 0, 0 };
	}
}

GlTexture::GlTexture(size_t width, size_t height, TextureFormat format) : width(width), height(height), id(0),
	useMipmaps(false), format(format)
{
	glGenTextures(1, &id);

	bind(0);
	auto f = glFromFormat(format);
	glTexImage2D(GL_TEXTURE_2D, 0, f.internalFormat, width, height, 0, f.format, f.type, nullptr);
}

void GlTexture::setSamplerParameters(TextureFilter magFilter, TextureFilter minFilter,
	TextureEdge edgeX, TextureEdge edgeY)
{
	bind(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFromTextureFilter(magFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFromTextureFilter(minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glFromTextureEdge(edgeX));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glFromTextureEdge(edgeY));
}

void GlTexture::setSamplerParameters(TextureFilter magFilter, TextureFilter minFilter, TextureFilter mipFilter,
	TextureEdge edgeX, TextureEdge edgeY)
{
	bind(0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFromTextureFilter(magFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFromTextureFilter(minFilter, mipFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glFromTextureEdge(edgeX));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glFromTextureEdge(edgeY));
}

void GlTexture::uploadData(const void* data)
{
	uploadSubData(data, util::irect(0, 0, width, height));
}

void GlTexture::uploadSubData(const void* data, util::irect rect)
{
	util::irect texRect(0, 0, width, height);
	if (!rect.intersects(texRect)) return;
	rect = rect.intersect(texRect);

	bind(0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	auto f = glFromFormat(format);
	glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, rect.width, rect.height, f.format, f.type, data);
}

void GlTexture::bind(size_t unit)
{
	if (unit >= prevTextureBinding.size()) return;

	if (prevActiveTexture != unit)
	{
		prevActiveTexture = unit;
		glActiveTexture(GL_TEXTURE0 + unit);
	}

	if (prevTextureBinding[unit] != id)
	{
		prevTextureBinding[unit] = id;
		glBindTexture(GL_TEXTURE_2D, id);
	}
}
