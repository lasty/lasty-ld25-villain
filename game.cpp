/*
 * game.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "game.h"

#include "os_utils.h"

#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
using std::vector;
using std::stringstream;
using std::map;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


using glm::vec3;
using glm::mat4;


class Object
{
public:

	glm::vec3 position;
	glm::vec3 rotation;

	glm::mat4 model_matrix;


	Prim * model;


	Object(vec3 position, vec3 rotation, Prim *model)
	: position(position), rotation(rotation), model(model)
	{
		UpdateMatrixes();
	}

	void UpdateMatrixes()
	{
		mat4 rotx = glm::rotate( mat4(), rotation.x, vec3(1.0f, 0.0f, 0.0f));
		mat4 roty = glm::rotate( mat4(), rotation.y, vec3(0.0f, 1.0f, 0.0f));
		mat4 rotz = glm::rotate( mat4(), rotation.z, vec3(0.0f, 0.0f, 1.0f));

		mat4 trans = glm::translate( mat4(), position);

		model_matrix = trans * rotx * roty * rotz;
	}

	void Render(Camera *cam, Program *prog)
	{
		if (model->img_ref) prog->SetTexture(model->img_ref);
		prog->SetModel(model_matrix);
		model->Draw();
	}
};


map<string, Prim*> PrimMap;
map<string, vec3> LightColours;

class Light
{
public:
	vec3 position;
	vec3 colour;

	Light(vec3 pos, vec3 colour)
	: position(pos), colour(colour)
	{

	}
};


vec3 colour_light_white(0.8f, 0.8f, 0.8f);
vec3 colour_light_red(0.8f, 0.2f, 0.2f);
vec3 colour_light_yellow(0.9f, 0.9f, 0.2f);
vec3 colour_light_cyan(0.2f, 0.8f, 0.9f);


vector<Object *> objs;

vector<Light *> lights;


vector<string> ParseTokens(string src)
{
	stringstream ss;
	ss << src;

	vector<string> toks;

	while (not ss.eof())
	{
		string tok;
		ss >> tok;

		if (tok != "") // and tok != "\n" and tok != "\r\n" and tok != "\n\r")
		{
			toks.push_back(tok);
		}
	}

	return toks;
}

vector<int> ParseCoords(string src)
{
	vector<int> toks;

	unsigned pos = 0;
	unsigned pos_next = 0;

	pos_next = src.find(",", pos);

	string xs = src.substr(0, pos_next);
	LOGf("xs = '%s'", xs.c_str());
	string ys = src.substr(pos_next + 1);
	LOGf("ys = '%s'", ys.c_str());

	toks.push_back(atoi(xs.c_str()));
	toks.push_back(atoi(ys.c_str()));

	return toks;
}

struct MapDataItem
{
	bool clip_player = false;
};


int mapsizex = 0;
int mapsizey = 0;
vector<vector<MapDataItem>> MapData;



bool ClipPlayer(glm::vec3 pos)
{
	int x = (pos.x / 2.0f);
	int y = (pos.z / 2.0f);

	//LOGf("ClipPlayer %.2f, %.2f -> %d, %d", pos.x, pos.z, x, y);

	if (x < 0 or x >= mapsizex) return true;
	if (y < 0 or y >= mapsizey) return true;

	MapDataItem d = MapData[y][x];

	return d.clip_player;
}


class Player
{
public:
	Object* player = nullptr;
	glm::vec3 player_vel;

	float player_intransit = 1.0f;
	vec3 player_destination;
	vec3 player_origin;
	float player_destination_rotation;

	bool control_fwd = false;
	bool control_back = false;
	bool control_left = false;
	bool control_right = false;

	Player(float x, float y)
	{
		Prim *pr = PrimMap["player"];
		player = new Object(vec3(x, 0.0f, y), vec3(0,0,0), pr);

		//player->position = vec3(x, 0, y);

		player_destination = player->position;
		player_destination_rotation = 0.0f;
	}

	~Player()
	{
		delete player;
	}

	void Update(float dt)
	{
		float playerspeed = 4.0f;

		player_vel = glm::vec3();

		if (control_fwd) player_vel.z -= 1.0f;
		if (control_back) player_vel.z += 1.0f;

		if (control_left) player_vel.x -= 1.0f;
		if (control_right) player_vel.x += 1.0f;

		if (player_intransit >= 1.0f and player_vel != vec3())
		{
			float new_rot;
			if (player_vel.z == 0.0f)
			{
				if (player_vel.x == -1.0f) new_rot = 90;
				else new_rot = 270;
			}
			else if (player_vel.z == 1.0f) new_rot = 180;
			else new_rot = 0.0f;

			player_destination_rotation = new_rot;

			if (not ClipPlayer(player->position + player_vel * 2.0f))
			{
				player_origin = player->position;
				player_destination = player->position + player_vel * 2.0f;
				player_intransit = 0.0f;
			}

		}

		if (player_intransit < 1.0f)
		{
			player_intransit += dt * playerspeed;
			player->position = glm::mix(player_origin, player_destination, player_intransit);
		}

		if (player_intransit > 1.0f)
		{
			player_intransit = 1.0f;
			player->position = player_destination;
		}

		player->position = glm::mix(player->position, player_destination, player_intransit);

		if (player->rotation.y - player_destination_rotation > 180.0f) player->rotation.y -= 360.0f;
		if (player->rotation.y - player_destination_rotation < -180.0f) player->rotation.y += 360.0f;

		player->rotation.y = glm::mix(player->rotation.y, player_destination_rotation, dt * 4.0f);

		player->UpdateMatrixes();

	}

};


class Enemy
{
public:


};


Player *the_player = nullptr;


void ClearLevel()
{
	for(auto i : objs)
	{
		delete i;
	}
	objs.clear();

	for (auto i : lights)
	{
		delete i;
	}
	lights.clear();

	MapData.clear();
	mapsizex = 0;
	mapsizey = 0;
}



void ParseLevel(string filename)
{
	ClearLevel();

	LOG("=== parsing level ===");
	stringstream ss;
	string src = ReadFile(filename);
	ss << src;

	string line;

	/// level name
	getline(ss, line);
	while (not StartsWith(line, "level "))
	{
		getline(ss, line);
	}

	map<char, string> legend;

	auto level_line = ParseTokens(line);

	LOGf("Level Name : %s", level_line.at(1).c_str());

	/// map legend
	getline(ss, line);
	while (not StartsWith(line, "legend ")) { getline(ss, line); }

	while (StartsWith(line, "legend "))
	{
		auto legend_line = ParseTokens(line);

		string sym = legend_line.at(1);
		string type = legend_line.at(2);

		legend[sym[0]] = type;

		LOGf("Legend : sym %s  ->  %s ", sym.c_str(), type.c_str());

		getline(ss, line);
	}

	/// map bitmap
	while (not StartsWith(line, "beginmap")) { getline(ss, line); }

	int row = 0;
	getline(ss, line);
	while (not StartsWith(line, "endmap"))
	{
		auto map_row_tok = ParseTokens(line);
		string map_row = map_row_tok.at(0);

		LOGf("Map line %d:  %s", row, map_row.c_str());

		vector<MapDataItem> mapdatarow;

		for (int x=0; x<int(map_row.size()); ++x)
		{
			string s = legend[map_row[x]];
			Object *obj = nullptr;

			Prim *pr = PrimMap[s];

			if (pr)
			{
				obj = new Object(vec3(x*2.0f,0,row*2.0f), vec3(0,0,0), pr);
				objs.push_back(obj);
			}
			else
			{
				LOGf("Unknown legend type '%s'", s.c_str());
				//THROW(string("Unknown legend type ") + s);
			}

			MapDataItem d;
			d.clip_player = s=="wall1";

			mapdatarow.push_back(d);
		}

		MapData.push_back(mapdatarow);
		mapsizey = MapData.size();
		mapsizex = mapdatarow.size();

		row++;
		getline(ss, line);
	}

	getline(ss, line);

	/// entity list
	for (;not ss.eof(); getline(ss, line))
	{
		auto entity_line = ParseTokens(line);

		LOGf("line '%s'", line.c_str());
		LOGf("num tokens %d", entity_line.size());
		if (entity_line.size() == 0) continue;

		string etype = entity_line.at(0);
		string epos_src = entity_line.at(1);
		auto epos = ParseCoords(epos_src);

		float x = epos.at(0) * 2;
		float y = epos.at(1) * 2;


		if (etype == "light")
		{
			string light_colour = entity_line.at(2);
			vec3 colour = LightColours[light_colour];

			lights.push_back( new Light(vec3(x, 1.0f, y), colour));
		}

		else if (etype == "playerstart" and the_player == nullptr)
		{
			the_player = new Player(x, y);
		}

		LOGf("Entity '%s' at '%s'", etype.c_str(), epos_src.c_str());
	}



	LOG("=== end parse ---");

}


Game::Game()
{

}



Game::~Game()
{

}

vec3 cam_offset(0, 2, 4);


void Game::InitGL()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	vbuff1 = new VertexArray();

	prog1 = new Program("../Data/prog1.glsl");
	prog2 = new ProgramLighting("../Data/prog_lighting.glsl");

	q1 = new Quad(*vbuff1, 1.0f);

	cam1 = new Camera();

	image_cell = new Image();
	image_cell->LoadImage("../Data/cell.webp");
	image_cell->SetSmooth();

	image_marble = new Image();
	image_marble->LoadImage("../Data/marble.webp");
	image_marble->SetSmooth();

	image_brick = new Image();
	image_brick->LoadImage("../Data/bricks.webp");
	image_brick->SetSmooth();


	cube1 = new ObjPrim(*vbuff1, "../Data/cube.obj", image_cell, 0.5f);
	PrimMap["cube1"] = cube1;

	player1 = new ObjPrim(*vbuff1, "../Data/arrow.obj", image_cell, 0.75f);
	PrimMap["player"] = player1;

	enemy1 = new ObjPrim(*vbuff1, "../Data/enemy.obj", image_cell, 0.75f);
	PrimMap["enemy"] = enemy1;
	//PrimMap["player"] = enemy1;

	light1 = new ObjPrim(*vbuff1, "../Data/cube.obj", image_cell, 0.1f);
	PrimMap["light1"] = light1;

	floor1 = new ObjPrim(*vbuff1, "../Data/floor.obj", image_marble);
	PrimMap["floor1"] = floor1;

	wall1 = new ObjPrim(*vbuff1, "../Data/wall.obj", image_brick);
	PrimMap["wall1"] = wall1;

	cam1->position += cam_offset;
	cam1->position_set += cam_offset;

	//initial height
	cam1->position.y += 5.0f;
	cam1->position_set.y += 5.0f;

	cam1->CalcMatrixes();

//	for (int x=-5; x<15; ++x)
//	{
//		for( int y=-4; y<14; ++y)
//		{
//			objs.push_back( new Object(vec3(x*2.0f,0,y*2.0f), vec3(0,0,0),
//			                           (x==-5 or x==14 or y==-4 or y==13) ? wall1 : floor1));
//		}
//	}

	LightColours["torch"] = vec3(1.0f, 0.8f, 0.2f);

	LightColours["yellow"] = colour_light_yellow * 5.0f;

	LightColours["red"] = colour_light_red * 5.0f;

	LightColours["cyan"] = colour_light_cyan * 5.0f;


	//player = new Object(vec3(0,0,0), vec3(0,0,0), cube1);
}

void Game::DestroyGL()
{
	ClearLevel();

	delete the_player;

	delete prog1;
	delete prog2;
	delete q1;
	delete vbuff1;

	delete image_cell;
	delete image_marble;
	delete image_brick;

	delete cube1;
	delete player1;
	delete enemy1;
	delete floor1;
	delete wall1;

	delete cam1;
}

void Game::Resize(int w, int h)
{
	LOGf("Resize: %dx%d", w, h);

	glViewport(0,0, w, h);
	cam1->Resize(w, h);
}

bool control_cam_up = false;
bool control_cam_down = false;

bool show_lights = false;
bool level_loaded = false;
bool sort_objects = false;

void Game::Update(float dt)
{
	if (not level_loaded)
	{
		ParseLevel("../Data/level1.txt");
		level_loaded=true;

		ASSERT(the_player);

	}


	the_player->Update(dt);

	float camspeed = 8.0f;

	cam1->look_at = the_player->player->position;
	cam1->position_set = the_player->player->position + cam_offset;

	vec3 cam_vel = glm::vec3();

	if (control_cam_up) cam_vel.y += 1.0f;
	if (control_cam_down) cam_vel.y -= 1.0f;

	cam1->position.y = cam1->position.y + ( cam_vel.y * camspeed * dt);

	float camfollowspeed = 1.25f;

	cam1->position.x = glm::mix(cam1->position.x, cam1->position_set.x, dt * camfollowspeed);
	cam1->position.z = glm::mix(cam1->position.z, cam1->position_set.z, dt * camfollowspeed);


	cam1->Update(dt);

}



void Game::Key(SDL_Keycode key, bool keydown)
{

	if (key == SDLK_w or key == SDLK_UP)
	{
		the_player->control_fwd = keydown;
	}

	if (key == SDLK_s or key == SDLK_DOWN)
	{
		the_player->control_back = keydown;
	}

	if (key == SDLK_a or key == SDLK_LEFT)
	{
		the_player->control_left = keydown;
	}

	if (key == SDLK_d or key == SDLK_RIGHT)
	{
		the_player->control_right = keydown;
	}

	if (key == SDLK_PLUS or key == SDLK_EQUALS or key == SDLK_KP_PLUS)
	{
		control_cam_up = keydown;
	}

	if (key == SDLK_MINUS or key == SDLK_KP_MINUS)
	{
		control_cam_down = keydown;
	}

	if (key == SDLK_ESCAPE)
	{
		running = false;
	}

	if (keydown and key == SDLK_F5)
	{
		level_loaded = false;  //reloads the level
	}

	if (keydown and key == SDLK_l)
	{
		show_lights = not show_lights;
		LOGf("%s lights", show_lights?"showing":"hiding");
	}

	if (keydown and key == SDLK_o)
	{
		sort_objects = not sort_objects;
		LOGf("%ssorting objects", sort_objects?"":"not ");
	}

}

struct light_sort
{
	Light* light;
	float distance;

	light_sort(Light* light, float distance)
	: light(light), distance(distance)
	{

	}

	bool operator<(const light_sort &rhs) const
	{
		return distance < rhs.distance;
	}
};

void SetLights(ProgramLighting *prog, vec3 pos)
{
	ASSERT(lights.size());

	vector<light_sort> light_bin;

	for (auto light : lights)
	{
		float distance = glm::distance(light->position, pos);

		light_bin.push_back(light_sort(light, distance));
	}

	std::sort(light_bin.begin(), light_bin.end());

	//LOGf("dist1 %.2f  dist2 %.2f dist 3 %.2f", light_bin[0].distance, light_bin[1].distance, light_bin[2].distance);
	ASSERT(light_bin[0].distance <= light_bin[1].distance);
	ASSERT( light_bin[1].distance <=  light_bin[2].distance);

	prog->SetLight(0, light_bin[0].light->position, light_bin[0].light->colour);
	prog->SetLight(1, light_bin[1].light->position, light_bin[1].light->colour);
	prog->SetLight(2, light_bin[2].light->position, light_bin[2].light->colour);

}


struct obj_sort
{
	Object* obj;
	float distance;

	bool operator<(const obj_sort &rhs) const
	{
		return distance < rhs.distance;
	}
};


void Game::RenderWorld()
{

	if (sort_objects) /// sort from front to back (pixel shader appears to be chewing up GPU time)
	{
		vector<obj_sort> obj_bin;
		for (auto i : objs)
		{
			float distance = glm::distance(i->position, cam1->position);

			obj_bin.push_back({i, distance});
		}

		std::sort(obj_bin.begin(), obj_bin.end());

		for (auto &i : obj_bin)
		{
			SetLights(prog2, i.obj->position);

			i.obj->Render(cam1, prog2);
		}
	}
	else
	{
		for (auto &i : objs)
		{
			SetLights(prog2, i->position);

			i->Render(cam1, prog2);
		}
	}


	if (show_lights)
	{
		for(auto l : lights)
		{
			prog1->Use();
			prog1->SetCamera(cam1);

			mat4 light_pos = glm::translate(mat4(), l->position);
			prog1->SetModel(light_pos);
			light1->Draw();
		}
	}
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog1->Use(vbuff1);
	prog1->SetCamera(cam1);
	prog1->SetModel(glm::mat4());


	prog1->SetTexture(image_marble);


	prog2->Use(vbuff1);
	prog2->SetCamera(cam1);
	prog2->SetTexture(image_marble);

	RenderWorld();


	prog2->Use(vbuff1);
	prog2->SetCamera(cam1);

	SetLights(prog2, the_player->player->position);

	prog2->SetTexture(image_cell);
	the_player->player->Render(cam1, prog2);


}

