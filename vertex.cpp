/*
 * vertex.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "vertex.h"
#include "globals.h"
#include "program.h"

VertexArray::VertexArray()
{

}

VertexArray::~VertexArray()
{
	//ClearPointers();
}

void VertexArray::SetPointers(Program *prog)
{
	constexpr auto stride = sizeof(vertex);

	ASSERT(prog->a_position != -1);
	//ASSERT(prog->a_texcoord != -1);
//	glVertexAttribPointer(prog->a_position, 3, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(vertex, x));
	glVertexAttribPointer(prog->a_position, 3, GL_FLOAT, GL_FALSE, stride, (void*) &operator[](0).x);

//	glVertexAttribPointer(prog->a_texcoord, 2, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(vertex, u));
	glVertexAttribPointer(prog->a_texcoord, 2, GL_FLOAT, GL_FALSE, stride, (void*) &operator[](0).u);

//	glVertexAttribPointer(prog->a_normal, 3, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(vertex, nx));
	glVertexAttribPointer(prog->a_normal, 3, GL_FLOAT, GL_FALSE, stride, (void*) &operator[](0).nx);

	glEnableVertexAttribArray(prog->a_position);
	glEnableVertexAttribArray(prog->a_texcoord);
	glEnableVertexAttribArray(prog->a_normal);
}

void VertexArray::ClearPointers(Program *prog)
{
	glDisableVertexAttribArray(prog->a_position);
	glDisableVertexAttribArray(prog->a_texcoord);
	glDisableVertexAttribArray(prog->a_normal);
}

void VertexArray::SetupArrays()
{
	//TODO: copy to VBO
}
