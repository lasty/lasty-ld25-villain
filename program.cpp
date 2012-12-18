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
	glDeleteShader(vs_id);
	glDeleteShader(fs_id);
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
	static int last_prog_used = -1;

	if (last_prog_used != prog_id)
	{
		Use();
		last_prog_used = prog_id;
	}


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

void Program::SetUniform(int uniform_id, const mat3 &m3)
{
	ASSERT(uniform_id != -1);
	glUniformMatrix3fv(uniform_id, 1, GL_FALSE, glm::value_ptr(m3));
}

void Program::SetUniform(int uniform_id, const vec3 &v3)
{
	ASSERT(uniform_id != -1);
	glUniform3fv(uniform_id, 1, glm::value_ptr(v3));
}

void Program::SetCamera(Camera *cam)
{
	//cam->CalcMatrixes();
	SetUniform(u_projection_view_matrix, cam->projection_view_matrix);
}

void Program::SetModel(const mat4 &model_matrix)
{
	SetUniform(u_model_matrix, model_matrix);
}

void Program::SetTexture(Image* img)
{
	//glActiveTexture(GL_TEXTURE0);  //not using multitexture, so remove redundant call

	static Image *last_texture = nullptr;  //cache last result, should be faster

	if (img != last_texture)
	{
		img->Bind();
		last_texture = img;
		if (u_tex != -1) SetUniform(u_tex, 0);
	}
}

ProgramLighting::ProgramLighting(const string filename)
: Program(filename)
{
	u_light1_col = glGetUniformLocation(prog_id, "light1_col");
	u_light1_pos = glGetUniformLocation(prog_id, "light1_pos");

	u_light2_col = glGetUniformLocation(prog_id, "light2_col");
	u_light2_pos = glGetUniformLocation(prog_id, "light2_pos");

	u_light3_col = glGetUniformLocation(prog_id, "light3_col");
	u_light3_pos = glGetUniformLocation(prog_id, "light3_pos");

	u_normal_matrix = glGetUniformLocation(prog_id, "normal_matrix");
	u_projection_matrix = glGetUniformLocation(prog_id, "projection_matrix");
	u_view_matrix = glGetUniformLocation(prog_id, "view_matrix");

}

void ProgramLighting::SetLight(int which, vec3 pos, vec3 col)
{
	if (which == 0)
	{
		if (u_light1_col != -1) SetUniform(u_light1_col, col);
		if (u_light1_pos != -1) SetUniform(u_light1_pos, pos);
	}
	else if (which == 1)
	{
		if (u_light2_col != -1) SetUniform(u_light2_col, col);
		if (u_light2_pos != -1) SetUniform(u_light2_pos, pos);
	}
	else if (which == 2)
	{
		if (u_light3_col != -1) SetUniform(u_light3_col, col);
		if (u_light3_pos != -1) SetUniform(u_light3_pos, pos);
	}
	else
	{
		THROW("light out of range");
	}
}

void ProgramLighting::SetCamera(Camera *cam)
{
	cam->CalcMatrixes();

	//mat3 normal_matrix(cam->view_matrix);

	if (u_projection_matrix != -1) SetUniform(u_projection_matrix, cam->projection_matrix);
	SetUniform(u_view_matrix, cam->view_matrix);

	if (u_projection_view_matrix != -1) SetUniform(u_projection_view_matrix, cam->projection_view_matrix);
}

void ProgramLighting::SetModel(const mat4 &model_matrix)
{
	SetUniform(u_model_matrix, model_matrix);

	glm::mat3 normal_matrix( model_matrix );

	SetUniform(u_normal_matrix, normal_matrix);
}

