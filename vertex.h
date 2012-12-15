/*
 * vertex.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#include "glm/glm.hpp"

#include "GL/glee.h"

#include <vector>
using std::vector;

class vertex
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float u = 0.5f;
	float v = 0.5f;

	float nx = 0.0f;  ///<< normal x
	float ny = 0.0f;  ///<< normal y
	float nz = 0.0f;  ///<< normal z

	vertex()
	{
	}

	vertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal)
	: x(pos.x), y(pos.y), z(pos.z), u(uv.s), v(uv.t), nx(normal.x), ny(normal.y), nz(normal.z)
	{

	}

};

class Program;

class VertexArray : public vector<vertex>
{
public:
	VertexArray();
	virtual ~VertexArray();

	void SetPointers(Program *);

	void ClearPointers(Program *);

	void SetupArrays();

};



#endif /* VERTEX_H_ */
