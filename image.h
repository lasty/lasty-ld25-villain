/*
 * image.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "GL/glee.h"
#include <string>
using std::string;


class Image
{
protected:
	friend class Program;
	GLuint tex_id;

public:
	Image();
	virtual ~Image();

	void Bind();


	void LoadImage(const string &filename);


	void SetSmooth();
};

#endif /* IMAGE_H_ */
