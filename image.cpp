/*
 * image.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "image.h"
#include "globals.h"

#include "os_utils.h"

#include "decode.h"


Image::Image()
{
	glGenTextures(1, &tex_id);


}

Image::~Image()
{
	glDeleteTextures(1, &tex_id);
}

void Image::Bind()
{
	glBindTexture(GL_TEXTURE_2D, tex_id);

}


void Image::LoadImage(const string &filename)
{
	Bind();

	string file_data = ReadFile(filename);

	int width;
	int height;

	const uint8_t* decoded = WebPDecodeRGBA((const uint8_t*)file_data.c_str(), file_data.size(), &width, &height);

	LOGf("image %s - %d x %d", filename.c_str(), width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, decoded);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//glGenerateMipmap(GL_TEXTURE_2D);

	CheckError();
}

void Image::SetSmooth()
{
	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

}

