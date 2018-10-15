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

#include <cstdint>

enum class TextureFormat
{
	UInt8N1, UInt8N2, UInt8N3, UInt8N4,
	UInt16N1, UInt16N2, UInt16N3, UInt16N4,

	Int8N1, Int8N2, Int8N3, Int8N4,
	Int16N1, Int16N2, Int16N3, Int16N4,

	UInt8I1, UInt8I2, UInt8I3, UInt8I4,
	UInt16I1, UInt16I2, UInt16I3, UInt16I4,
	UInt32I1, UInt32I2, UInt32I3, UInt32I4,

	Int8I1, Int8I2, Int8I3, Int8I4,
	Int16I1, Int16I2, Int16I3, Int16I4,
	Int32I1, Int32I2, Int32I3, Int32I4,

	Float1, Float2, Float3, Float4,

	RGB10A2,

	NumTextureFormats
};

enum class TextureFilter : uint8_t { Nearest, Linear };
enum class TextureEdge : uint8_t { Wrap, Mirror, Clamp };

#include "gl/Texture.hpp"
using Texture = GlTexture;
