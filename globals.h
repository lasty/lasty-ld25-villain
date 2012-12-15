/*
 * globals.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdio.h>
#include <string>

class Exception
{
public:
	std::string what;
	std::string file;
	int line;

	Exception(const std::string &what, const char* file, int line)
	: what(what), file(file), line(line)
	{

	}

	Exception(const std::string &what)
	: what(what), file(""), line(0)
	{

	}
};


#define THROW(what) throw Exception(what, __FILE__, __LINE__)


#define LOG(msg) {printf(msg " [%s : %d]\n", __FILE__, __LINE__);fflush(stdout);}
#define LOGf(msg, ...) {printf(msg " [%s : %d]\n", __VA_ARGS__, __FILE__, __LINE__);fflush(stdout);}

#define ASSERT(test) if(not (test)) THROW(#test)
#define ASSERTf(test, msg) if(not (test)) THROW(msg)


extern const std::string TITLE;
extern int WIDTH;
extern int HEIGHT;



#endif /* GLOBALS_H_ */
