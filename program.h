/*
 * program.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <string>
using std::string;

#include "camera.h"

class VertexArray;
class Image;

class Program
{
protected:
	friend class VertexArray;

	string fs_source;
	string vs_source;

	int prog_id = -1;
	int vs_id = -1;
	int fs_id = -1;

	int a_position = -1;
	int a_texcoord = -1;
	int a_normal = -1;

	int u_projection_view_matrix = -1;
	int u_model_matrix = -1;

	int u_tex = -1;

public:
	Program(const string filename);
	virtual ~Program();

	string GetLog(int shader_id);
	string GetLinkLog(int prog_id);

	void GetVariables();


	void Use();
	void Use(VertexArray *vbuff);

	void SetUniform(int uniform_id, const mat4 &m4);
	void SetUniform(int uniform_id, int i1);


	void SetCamera(Camera *cam);
	void SetModel(const mat4 &model_matrix);

	void SetTexture(Image* img);
};

#endif /* PROGRAM_H_ */
