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


#include "GlTexturedVertexArray.hpp"

// We'll let the context destroy the shader


GlTexturedVertexArray::GlTexturedVertexArray() : elementBuffer(0)
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertexBuffer);
}

GlTexturedVertexArray::~GlTexturedVertexArray()
{
	if (vao) glDeleteVertexArrays(1, &vao);
	if (vertexBuffer) glDeleteBuffers(1, &vertexBuffer);
	if (elementBuffer) glDeleteBuffers(1, &elementBuffer);
}

void GlTexturedVertexArray::setVertices(const TVertex* vertices, size_t numVertices)
{
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex)*numVertices, vertices, GL_DYNAMIC_DRAW);
}

void GlTexturedVertexArray::setIndices(const uint16_t* indices, size_t numIndices)
{
	glBindVertexArray(vao);

	if (indices)
	{
		if (!elementBuffer) glGenBuffers(1, &elementBuffer);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t)*numIndices, indices, GL_DYNAMIC_DRAW);
	}
	else
	{
		if (elementBuffer)
		{
			glDeleteBuffers(1, &elementBuffer);
			elementBuffer = 0;
		}
	}
}

void GlTexturedVertexArray::draw(const glm::mat3& projTransform)
{

}