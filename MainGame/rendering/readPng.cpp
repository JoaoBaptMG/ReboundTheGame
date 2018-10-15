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


#include "readPng.hpp"
#include "Texture.hpp"

#include <png.h>
#include <cstdint>
#include <vector>
#include <csetjmp>
#include <numeric>
#include <memory>

util::generic_shared_ptr loadPngTexture(std::unique_ptr<InputStream>& stream)
{
	png_byte pngHeader[8];
	if (stream->read(pngHeader, sizeof(pngHeader)) != sizeof(pngHeader))
		return util::generic_shared_ptr{};

	if (png_sig_cmp(pngHeader, 0, 8)) return util::generic_shared_ptr{};

	auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png) return util::generic_shared_ptr{};

	auto info = png_create_info_struct(png);
	if (!info) return util::generic_shared_ptr{};

	std::vector<png_byte> buffer;
	std::vector<png_bytep> rowAddrs;

	// I really don't know what are the implications of setjmp in a C++ project
	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_read_struct(&png, &info, nullptr);
		return util::generic_shared_ptr{};
	}

	// Set the read function
	png_set_read_fn(png, &stream, [](png_structp png, png_bytep data, png_size_t size)
	{
		InputStream& stream = *(InputStream*)png_get_io_ptr(png);
		if (stream.read(data, size) != size) png_error(png, "Error while reading the PNG stream!");
	});

	png_set_sig_bytes(png, 8);
	png_read_info(png, info);

	auto width = png_get_image_width(png, info);
	auto height = png_get_image_height(png, info);
	auto colorType = png_get_color_type(png, info);
	auto bitDepth = png_get_bit_depth(png, info);
	auto channels = png_get_channels(png, info);

	switch (colorType) 
	{
	case PNG_COLOR_TYPE_PALETTE: png_set_palette_to_rgb(png); channels = 3; break;
	case PNG_COLOR_TYPE_GRAY: if (bitDepth < 8) png_set_expand_gray_1_2_4_to_8(png); bitDepth = 8; break;
	}

	if (png_get_valid(png, info, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png);
		channels += 1;
	}

	if (bitDepth > 8)
	{
		png_set_strip_16(png);
		bitDepth = 8;
	}

	png_read_update_info(png, info);

	auto rowBytes = png_get_rowbytes(png, info);
	buffer.resize(height * rowBytes);
	rowAddrs.resize(height);
	for (int i = 0; i < height; i++) rowAddrs[i] = buffer.data() + (rowBytes*i);

	png_read_image(png, rowAddrs.data());
	png_destroy_read_struct(&png, &info, nullptr);

	TextureFormat format;
	switch (channels)
	{
	case 1:  format = TextureFormat::UInt8N1; break;
	case 2:  format = TextureFormat::UInt8N2; break;
	case 3:  format = TextureFormat::UInt8N3; break;
	default: format = TextureFormat::UInt8N4; break;
	}

	auto texture = std::make_shared<Texture>(width, height, format);
	texture->setSamplerParameters(TextureFilter::Nearest, TextureFilter::Nearest, TextureEdge::Wrap, TextureEdge::Wrap);
	texture->uploadData(buffer.data());

	return util::generic_shared_ptr{ std::move(texture) };
}