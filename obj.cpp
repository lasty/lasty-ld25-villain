/*
 * obj.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

constexpr bool debug = false;

#include "obj.h"

#include "globals.h"
#include "os_utils.h"

#include "glm/glm.hpp"

#include <sstream>
#include <vector>
using std::stringstream;
using std::vector;

ObjPrim::ObjPrim(VertexArray &vbuff, string filename, float zoom)
:Prim(vbuff)
{
	ParseFile(filename, zoom);
}


ObjPrim::~ObjPrim()
{

}

bool StartsWith(const string& s, const string& token)
{
	return s.substr(0, token.size()) == token;
}


glm::vec3 ParseLine3(string & line)
{
	stringstream ss;
	ss << line;

	float x, y, z;

	string tok;

	ss >> tok >> x >> y >> z;

	if (debug) LOGf("Line3:  tok(%s), x(%.2f), y(%.2f), z(%.2f)", tok.c_str(), x, y, z);

	return glm::vec3(x,y,z);
}

glm::vec2 ParseLine2(string & line)
{
	stringstream ss;
	ss << line;

	float x, y;

	string tok;

	ss >> tok >> x >> y;

	if (debug) LOGf("Line2:  tok(%s), u(%.2f), v(%.2f)", tok.c_str(), x, y);

	return glm::vec2(x,y);
}

void ParseFLine(const string& line, int &pos_index, int &uv_index, int &normal_index)
{
	auto slash1 = line.find("/");
	auto slash2 = line.find("/", slash1+1);

	string v1 = line.substr(0, slash1);
	string v2 = line.substr(slash1+1, slash2-slash1-1);
	string v3 = line.substr(slash2+1);

	pos_index = atoi(v1.c_str());
	uv_index = atoi(v2.c_str());
	normal_index = atoi(v3.c_str());

	if (debug) LOGf("  %s   /   %s   /   %s  ", v1.c_str(), v2.c_str(), v3.c_str());

}


void ObjPrim::ParseFile(string filename, float zoom)
{
	stringstream in;
	string in_src = ReadFile(filename);
	in << in_src;

	string line;

	std::getline(in, line);

	while (not StartsWith(line, "o"))
	{
		std::getline(in, line);
	}

	if (debug) LOGf("%s", line.c_str());


	vector<glm::vec3> positions;
	vector<glm::vec2> uvcoords;
	vector<glm::vec3> normals;


	std::getline(in, line);
	while (StartsWith(line, "v "))
	{
		if (debug) LOGf("v -- %s", line.c_str());
		positions.push_back(zoom * ParseLine3(line));

		std::getline(in, line);
	}

	while (StartsWith(line, "vt "))
	{
		if (debug) LOGf("vt -- %s", line.c_str());
		uvcoords.push_back(ParseLine2(line));

		std::getline(in, line);
	}

	while (StartsWith(line, "vn "))
	{
		if (debug) LOGf("vn -- %s", line.c_str());
		normals.push_back(ParseLine3(line));

		std::getline(in, line);
	}


	while (not StartsWith(line, "f "))
	{
		std::getline(in, line);
	}

	Begin(GL_TRIANGLES);

	while (StartsWith(line, "f "))
	{
		if (debug) LOGf("f -- %s", line.c_str());

		string tok;
		stringstream fline;
		fline << line;

		fline >> tok;

		string vv1, vv2, vv3;
		fline >> vv1 >> vv2 >> vv3;

		int p, u, n;

		ParseFLine(vv1, p, u, n);

		//vertex V1(positions[p-1], uvcoords[u-1], normals[n-1]);
		vertex V1(positions.at(p-1), uvcoords.at(u-1), normals.at(n-1));

		ParseFLine(vv2, p, u, n);
		vertex V2(positions.at(p-1), uvcoords.at(u-1), normals.at(n-1));

		ParseFLine(vv3, p, u, n);
		vertex V3(positions.at(p-1), uvcoords.at(u-1), normals.at(n-1));

		Vertex(V1);
		Vertex(V2);
		Vertex(V3);


//		LOGf("%d, %d, %d", p, u, n);

		//LOGf("%s", vv2.c_str());
		//LOGf("%s", vv3.c_str());

		std::getline(in, line);
	}

	End();

}

