/*
 * os_utils.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *	Platform specific ways of doing things
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef OS_UTILS_H_
#define OS_UTILS_H_

#include <string>

using std::string;


void ChangeToExecutableDirectory(int argc, char* argv[]);


void PopupMessageBox(string msg);

void PopupErrorBox(string msg);

string ReadFile(const string &filename);

bool StartsWith(const string& s, const string& token);


#endif /* OS_UTILS_H_ */
