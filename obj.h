/*
 * obj.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef OBJ_H_
#define OBJ_H_

#include "prim.h"
#include <string>
using std::string;

class ObjPrim : public Prim
{
public:
	ObjPrim(VertexArray &vbuff, string filename, float zoom=1.0f);
	virtual ~ObjPrim();

	void ParseFile(string filename, float zoom);
};

#endif /* OBJ_H_ */
