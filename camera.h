/*
 * camera.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include "glm/glm.hpp"

using glm::mat4;
using glm::vec3;


class Camera
{
public:
	vec3 look_at;
	vec3 position;
	vec3 up;

	float fov = 45.0f;
	float aspect = 1.0f;
	float znear = 0.1f;
	float zfar = 100.0f;

	mat4 projection_matrix;
	mat4 view_matrix;

	mat4 projection_view_matrix;

	Camera();

	void CalcMatrixes();

	void Resize(int w, int h);

	virtual ~Camera();
};

#endif /* CAMERA_H_ */
