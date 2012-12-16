/*
 * os_utils.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "os_utils.h"
#include "globals.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
//#include "unistd.h"

#include <sstream>
#include <fstream>


void ChangeToExecutableDirectory(int argc, char* argv[])
{
	string a0(argv[0]);
	string path = a0.substr(0, a0.find_last_of("\\"));
	//PopupMessageBox(path);

	SetCurrentDirectory(path.c_str());
}


void PopupMessageBox(string msg)
{
	MessageBox(nullptr, msg.c_str(), TITLE.c_str(), MB_OK);
}

void PopupErrorBox(string msg)
{
	MessageBox(nullptr, msg.c_str(), TITLE.c_str(), MB_ICONERROR | MB_OK);
}


string ReadFile(const string &filename)
{
	std::stringstream ins;
	std::fstream inf(filename, std::ios::in | std::ios::binary);

	ASSERTf(inf.good(), string("Could not open file: ") + filename);
	ins << inf.rdbuf();

	return ins.str();
}


bool StartsWith(const string& s, const string& token)
{
	return s.substr(0, token.size()) == token;
}

