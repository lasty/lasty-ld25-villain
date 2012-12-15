/*
 * program.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "program.h"
#include "globals.h"
#include "os_utils.h"

#include "vertex.h"
#include "image.h"

#include "GL/glee.h"

#include "glm/gtc/type_ptr.hpp"

#include <sstream>
using std::stringstream;
using std::endl;

Program::Program(const string filename)
{

	string source = ReadFile(filename);
	const string token = "%%%%";
	auto pos = source.find(token);

	vs_source = source.substr(0, pos);
	fs_source = source.substr(pos + token.size());

	//PopupMessageBox(vs_source);
	//PopupMessageBox(fs_source);

	prog_id = glCreateProgram();

	vs_id = glCreateShader(GL_VERTEX_SHADER);
	fs_id = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vsrc[] = {vs_source.c_str()};
	const char* fsrc[] = {fs_source.c_str()};

	glShaderSource(vs_id, 1, vsrc, nullptr);
	glShaderSource(fs_id, 1, fsrc, nullptr);

	glCompileShader(vs_id);
	int status;

	glGetShaderiv(vs_id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		THROW(GetLog(vs_id));
	}

	glCompileShader(fs_id);
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		THROW(GetLog(fs_id));
	}

	glAttachShader(prog_id, vs_id);
	glAttachShader(prog_id, fs_id);

	glLinkProgram(prog_id);

	glGetProgramiv(prog_id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		THROW(GetLinkLog(prog_id));
	}

	CheckError();

	GetVariables();
	CheckError();

}

Program::~Program()
{
	glDetachShader(prog_id, vs_id);
	glDetachShader(prog_id, fs_id);
	glDeleteProgram(prog_id);
}

string Program::GetLog(int shader_id)
{
	stringstream log;
	log << "ERROR COMPILING SHADER:" << endl << endl;

	int log_len;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_len);

	GLchar logbuf[log_len + 1];

	glGetShaderInfoLog(shader_id, log_len, nullptr, logbuf);

	log << logbuf << endl;

	return log.str();
}

string Program::GetLinkLog(int prog_id)
{
	stringstream log;
	log << "ERROR LINKING SHADER:" << endl << endl;

	int log_len;
	glGetProgramiv(prog_id, GL_INFO_LOG_LENGTH, &log_len);

	GLchar logbuf[log_len + 1];

	glGetProgramInfoLog(prog_id, log_len, nullptr, logbuf);

	log << logbuf << endl;

	return log.str();
}


void Program::GetVariables()
{
	a_position = glGetAttribLocation(prog_id, "position");
	a_texcoord = glGetAttribLocation(prog_id, "texcoord");
	a_normal = glGetAttribLocation(prog_id, "normal");

	u_projection_view_matrix = glGetUniformLocation(prog_id, "projection_view_matrix");
	u_model_matrix = glGetUniformLocation(prog_id, "model_matrix");

	u_tex = glGetUniformLocation(prog_id, "tex");
}


void Program::Use()
{
	glUseProgram(prog_id);
}

void Program::Use(VertexArray *vbuff)
{
	Use();

	vbuff->SetPointers(this);
}

void Program::SetUniform(int uniform_id, const mat4 &m4)
{
	ASSERT(uniform_id != -1);
	glUniformMatrix4fv(uniform_id, 1, GL_FALSE, glm::value_ptr(m4));
}

void Program::SetUniform(int uniform_id, int i1)
{
	ASSERT(uniform_id != -1);
	glUniform1i(uniform_id, i1);
}

void Program::SetCamera(Camera *cam)
{
	cam->CalcMatrixes();
	SetUniform(u_projection_view_matrix, cam->projection_view_matrix);
}

void Program::SetModel(const mat4 &model_matrix)
{
	SetUniform(u_model_matrix, model_matrix);
}


void Program::SetTexture(Image* img)
{
	img->Bind();
	SetUniform(u_tex, 0);
}



