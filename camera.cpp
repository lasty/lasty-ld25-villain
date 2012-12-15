/*
 * camera.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera()
{
	position = vec3(10.0f, 2.0f, 10.0f);
	look_at = vec3(0.0f, 0.0f, 0.0f);
	up = vec3(0.0f, 1.0f, 0.0f);

	CalcMatrixes();
}

void Camera::CalcMatrixes()
{
	projection_matrix = glm::perspective(fov, aspect, znear, zfar);

	view_matrix = glm::lookAt(position, look_at, up);

	projection_view_matrix = projection_matrix * view_matrix;
}

void Camera::Resize(int w, int h)
{
	if (h == 0) h = 1;
	aspect = float(w) / float(h);

	CalcMatrixes();
}


Camera::~Camera()
{

}

