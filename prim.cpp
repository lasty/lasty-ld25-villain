/*
 * prim.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "prim.h"

Prim::Prim(VertexArray &vtxbuff_ref)
:vbuff_ref(vtxbuff_ref)
{

}

Prim::~Prim()
{

}

void Prim::Begin(int prim_type)
{
	this->prim_type = prim_type;

	offset = vbuff_ref.size();

}

void Prim::Vertex(const vertex &v)
{
	vbuff_ref.push_back(v);
	num++;
}

void Prim::End()
{
	vbuff_ref.SetupArrays();
}

void Prim::Draw()
{
	glDrawArrays(prim_type, offset, num);
}



Quad::Quad(VertexArray &vtxarr, float size)
:Prim(vtxarr)
{
	vertex v1;
	v1.x = -size;  v1.y = -size;
	v1.u = 0.0f;  v1.v = 1.0f;

	vertex v2;
	v2.x = size;  v2.y = -size;
	v2.u = 1.0f;  v2.v = 1.0f;

	vertex v3;
	v3.x = -size;  v3.y = size;
	v3.u = 0.0f;  v3.v = 0.0f;

	vertex v4;
	v4.x = size;  v4.y = size;
	v4.u = 1.0f;  v4.v = 0.0f;


	Begin(GL_TRIANGLES);

	Vertex(v2);
	Vertex(v1);
	Vertex(v3);

	Vertex(v2);
	Vertex(v3);
	Vertex(v4);

	End();
}


