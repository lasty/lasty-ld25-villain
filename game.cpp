/*
 * game.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

// This file seems to have everything dumped in it
// Didnt have time to separate it properly :/

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

	virtual ~Object() { }

	void UpdateMatrixes()
	{
		mat4 rotx = glm::rotate( mat4(), rotation.x, vec3(1.0f, 0.0f, 0.0f));
		mat4 roty = glm::rotate( mat4(), rotation.y, vec3(0.0f, 1.0f, 0.0f));
		mat4 rotz = glm::rotate( mat4(), rotation.z, vec3(0.0f, 0.0f, 1.0f));

		mat4 trans = glm::translate( mat4(), position);

		model_matrix = trans * rotx * roty * rotz;
	}

	virtual void Render(Camera *cam, Program *prog)
	{
		if (model->img_ref) prog->SetTexture(model->img_ref);
		prog->SetModel(model_matrix);
		model->Draw();
	}

	virtual void Update(float dt) { }
};

class Loot : public Object
{
public:
	Loot(vec3 position, vec3 rotation, Prim *model)
	: Object(position, rotation, model)
	{
		UpdateMatrixes();
	}

	void Update(float dt)
	{
		float rotatespeed = 90.0f;

		rotation.y += rotatespeed * dt;
		if (rotation.y > 360.0f) rotation.y -= 360.0f;

		UpdateMatrixes();
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


vector<Object *> objs;  ///< static, dont get updated

vector<Object *> entities;  ///< dynamic, update(dt) gets called

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
	//LOGf("xs = '%s'", xs.c_str());
	string ys = src.substr(pos_next + 1);
	//LOGf("ys = '%s'", ys.c_str());

	toks.push_back(atoi(xs.c_str()));
	toks.push_back(atoi(ys.c_str()));

	return toks;
}

struct MapDataItem
{
	bool clip_player = false;
	bool occupied = false;

	Loot * has_loot = nullptr;
};


int mapsizex = 0;
int mapsizey = 0;
vector<vector<MapDataItem>> MapData;


int current_score = 0;
int max_score = 0;

bool level_won = true;
bool level_lost = true;


bool ClipPlayer(vec3 pos)
{
	int x = (pos.x / 2.0f);
	int y = (pos.z / 2.0f);

	//LOGf("ClipPlayer %.2f, %.2f -> %d, %d", pos.x, pos.z, x, y);

	if (x < 0 or x >= mapsizex) return true;
	if (y < 0 or y >= mapsizey) return true;

	MapDataItem d = MapData[y][x];

	return d.clip_player or d.occupied;
}

void Occupy(vec3 pos)
{
	int x = (pos.x / 2.0f);
	int y = (pos.z / 2.0f);

	//LOGf("ClipPlayer %.2f, %.2f -> %d, %d", pos.x, pos.z, x, y);

	if (x < 0 or x >= mapsizex) return;
	if (y < 0 or y >= mapsizey) return;

	MapDataItem &d = MapData[y][x];

	if (d.occupied) LOGf("Occupy Warning trying to occupy a spot already occupied (%d, %d)", x, y);

	d.occupied = true;
}

void Unoccupy(vec3 pos)
{
	int x = (pos.x / 2.0f);
	int y = (pos.z / 2.0f);

	//LOGf("ClipPlayer %.2f, %.2f -> %d, %d", pos.x, pos.z, x, y);

	if (x < 0 or x >= mapsizex) return;
	if (y < 0 or y >= mapsizey) return;

	MapDataItem &d = MapData[y][x];

	if (not d.occupied) LOGf("Unoccupy Warning trying to free a spot already freed (%d, %d)", x, y);

	d.occupied = false;
}


MapDataItem & GetData(int x, int y)
{
	if (x < 0 or x >= mapsizex) THROW("map data get x out of range");
	if (y < 0 or y >= mapsizey) THROW("map data get y out of range");

	MapDataItem &d = MapData[y][x];

	return d;
}


MapDataItem & GetData(vec3 pos)
{
	int x = (pos.x / 2.0f);
	int y = (pos.z / 2.0f);

	return GetData(x, y);
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

	float playerspeed = 4.0f;

	float emote_time = 0.0f;

	Player()
	{
		//base default constructor - initialize everything in subclass Enemy below
	}

	Player(float x, float y)
	{
		Prim *pr = PrimMap["player"];
		player = new Object(vec3(x, 0.0f, y), vec3(0,0,0), pr);

		//player->position = vec3(x, 0, y);

		player_destination = player->position;
		player_destination_rotation = 0.0f;
		Occupy(player_destination);
	}

	virtual ~Player()
	{
		delete player;
	}

	void Update(float dt)
	{
		emote_time -= dt;
		if (emote_time < 0.0f) emote_time = 0.0f;

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

				Unoccupy(player_origin);
				Occupy(player_destination);
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

			//finished transit, apply any checks

			EnteredSquare(player->position);
		}

		player->position = glm::mix(player->position, player_destination, player_intransit);

		if (player->rotation.y - player_destination_rotation > 180.0f) player->rotation.y -= 360.0f;
		if (player->rotation.y - player_destination_rotation < -180.0f) player->rotation.y += 360.0f;

		player->rotation.y = glm::mix(player->rotation.y, player_destination_rotation, dt * 4.0f);

		player->UpdateMatrixes();

	}

	virtual void EnteredSquare(vec3 position)
	{
		MapDataItem &data = GetData(player->position);

		if (data.has_loot)
		{
			Loot * l = data.has_loot;
			entities.erase(std::find(entities.begin(), entities.end(), l));

			delete l;

			current_score ++;

			PlaySound("loot");

			emote_time = 2.0f;

			data.has_loot = nullptr;
		}
	}


};

Player *the_player = nullptr;

class Enemy : public Player
{
public:

	Enemy(float x, float y, string initial_dir)
	: Player()
	{
		Prim *pr = PrimMap["enemy"];
		player = new Object(vec3(x, 0.0f, y), vec3(0, 0, 0), pr);

		player_destination = player->position;
		player_destination_rotation = 0.0f;
		Occupy(player_destination);

		playerspeed = 1.0f;

		if (initial_dir == "N") dir = 0;
		if (initial_dir == "E") dir = 1;
		if (initial_dir == "S") dir = 2;
		if (initial_dir == "W") dir = 0;

		thinking_delay = 1.0f;
		player_intransit = 1.0f;

	}

	int dir = 0;
	bool on_alert = false;


	bool waiting_cmd = false;
	float thinking_delay;

	void AI(float dt)
	{
		constexpr bool debug = false;

		float delay1 = on_alert ? 0.2f : 0.4f;
		float delay2 = on_alert ? 0.4f : 1.4f;
		playerspeed = on_alert ? 4.0f : 1.0f;

		//LOGf("AI - waiting_cmd = %s  transit = %.2f ", waiting_cmd ? "true":"false", player_intransit);

		SearchForPlayer();

		if (waiting_cmd) return;

		thinking_delay -= dt;

		if (thinking_delay > 0.0f) return;

		vec3 newpos = player->position;

		if (dir == 0) { newpos.z -= 2.0f;  player_destination_rotation = 0.0f; }
		else if (dir == 1) { newpos.x += 2.0f;  player_destination_rotation = 270.0f; }
		else if (dir == 2) { newpos.z += 2.0f;  player_destination_rotation = 180.0f; }
		else if (dir == 3) { newpos.x -= 2.0f;  player_destination_rotation = 90.0f; }

		if (ClipPlayer(newpos))
		{
			if (IsPlayerAt(newpos))  //if a guard is touching a player
			{
				level_lost = true;
				PlaySound("lose");
			}

			dir = (dir+1) % 4;
			if (debug) LOGf("AI command:  rotate to direction %d", dir);
			player_destination_rotation -= 90.0f;
			thinking_delay = delay2;
			return;
		}

		if (debug) LOGf("AI command:  move in direction %d", dir);
		player_origin = player->position;
		player_destination = newpos;
		player_intransit = 0.0f;
		thinking_delay = delay1;
		waiting_cmd = true;

		Unoccupy(player_origin);
		Occupy(player_destination);

	}

	bool IsPlayerAt(vec3 newpos)
	{
		int playerx = the_player->player->position.x/2.0f;
		int playery = the_player->player->position.z/2.0f;

		int thisx = newpos.x / 2.0f;
		int thisy = newpos.z / 2.0f;

		return (thisx == playerx and thisy == playery);
	}

	void SearchForPlayer()
	{
		vec3 newpos = player->position;

		int max_checks = 10;

		for (int i=0; i< max_checks; ++i)
		{

			if (dir == 0) { newpos.z -= 2.0f; }
			else if (dir == 1) { newpos.x += 2.0f; }
			else if (dir == 2) { newpos.z += 2.0f; }
			else if (dir == 3) { newpos.x -= 2.0f; }

			//MapDataItem &d = GetData(newpos);


			if (IsPlayerAt(newpos))
			{
				emote_time = 2.0f;

				thinking_delay = 0.0f;

				return;
			}

			if (ClipPlayer(newpos)) return;

		}

	}

	void Update(float dt)
	{
		AI(dt);

		Player::Update(dt);

		on_alert = emote_time > 0.0f;

		waiting_cmd = player_intransit < 1.0f;
	}

	void EnteredSquare(vec3 position)
	{
		//do nothing, stops AI from picking up loot
	}

};


vector<Enemy*> Enemies;


void ClearLevel()
{
	current_score = 0;
	max_score = 0;

	level_won = false;
	level_lost = false;

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

	for (auto e : Enemies)
	{
		delete e;
	}
	Enemies.clear();

	for (auto e : entities)
	{
		delete e;
	}
	entities.clear();

	MapData.clear();
	mapsizex = 0;
	mapsizey = 0;
}


void AddLoot(string etype, int x, int y, MapDataItem &d)
{

	max_score++;

	Prim *pr = PrimMap[etype];

	Loot *obj = new Loot(vec3(x*2,0,y*2), vec3(0,0,0), pr);

	entities.push_back(obj);

	d.has_loot = obj;
}


void ParseLevel(string filename)
{
	constexpr bool debug = false;

	ClearLevel();

	if (debug) LOG("=== parsing level ===");
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

	if (debug) LOGf("Level Name : %s", level_line.at(1).c_str());

	/// map legend
	getline(ss, line);
	while (not StartsWith(line, "legend ")) { getline(ss, line); }

	while (StartsWith(line, "legend "))
	{
		auto legend_line = ParseTokens(line);

		string sym = legend_line.at(1);
		string type = legend_line.at(2);

		legend[sym[0]] = type;

		if (debug) LOGf("Legend : sym %s  ->  %s ", sym.c_str(), type.c_str());

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

		if (debug) LOGf("Map line %d:  %s", row, map_row.c_str());

		vector<MapDataItem> mapdatarow;

		for (int x=0; x<int(map_row.size()); ++x)
		{
			mapdatarow.push_back(MapDataItem());
			MapDataItem &d = mapdatarow.back();

			string s = legend[map_row[x]];
			Object *obj = nullptr;

			float fx = x * 2.0f;
			float fy = row * 2.0f;

			if (s == "goldbars" or s == "cash")
			{
				AddLoot(s, x, row, d);

				// add floor underneath
				s = "floor1";
			}

			if (s == "light1" or s == "light2" or s == "light3")
			{
				vec3 colour(0.8f, 0.8f, 0.8f);

				if (s == "light1") colour = LightColours["torch"];
				if (s == "light2") colour = LightColours["cyan"];
				if (s == "light3") colour = LightColours["red"];

				lights.push_back( new Light(vec3(fx, 1.0f, fy), colour));

				s = "floor1";
			}

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

			d.clip_player = s=="wall1";

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

		if (debug) LOGf("line '%s'", line.c_str());
		if (debug) LOGf("num tokens %d", entity_line.size());
		if (entity_line.size() == 0) continue;

		string etype = entity_line.at(0);
		string epos_src = entity_line.at(1);
		auto epos = ParseCoords(epos_src);

		float x = epos.at(0) * 2;
		float y = epos.at(1) * 2;
		int ix = epos.at(0);
		int iy = epos.at(1);


		if (etype == "light")
		{
			string light_colour = entity_line.at(2);
			vec3 colour = LightColours[light_colour];

			lights.push_back( new Light(vec3(x, 1.0f, y), colour));
		}

		else if (etype == "playerstart")
		{
			if (the_player != nullptr)
			{
				delete the_player;
			}
			the_player = new Player(x, y);
		}

		else if (etype == "enemy")
		{
			string dir = entity_line.size() > 2 ? entity_line.at(2) : "N";

			Enemies.push_back( new Enemy(x,y, dir) );
		}

		else if (etype == "goldbars" or etype == "cash")
		{
			AddLoot(etype, ix, iy, GetData(ix, iy));
		}

		if (debug) LOGf("Entity '%s' at '%s'", etype.c_str(), epos_src.c_str());
	}


	if (debug) LOG("=== end parse ---");

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

	//y0 and y1 values are screwed here, no time to investigate properly

	ui_loot = new Quad(*vbuff1, 8, 0, 7, 4, 8);
	ui_win = new Quad(*vbuff1, 8, 0, 13, 6, 14);
	ui_caught = new Quad(*vbuff1, 8, 0, 3, 6, 4);
	ui_1 = new Quad(*vbuff1, 8, 4, 7, 5, 8);
	ui_2 = new Quad(*vbuff1, 8, 5, 7, 6, 8);
	ui_3 = new Quad(*vbuff1, 8, 6, 7, 7, 8);
	ui_4 = new Quad(*vbuff1, 8, 7, 7, 8, 8);

	ui_5 = new Quad(*vbuff1, 8, 0, 9, 1, 10);
	ui_6 = new Quad(*vbuff1, 8, 1, 9, 2, 10);
	ui_7 = new Quad(*vbuff1, 8, 2, 9, 3, 10);
	ui_8 = new Quad(*vbuff1, 8, 3, 9, 4, 10);
	ui_9 = new Quad(*vbuff1, 8, 4, 9, 5, 10);
	ui_0 = new Quad(*vbuff1, 8, 5, 9, 6, 10);
	ui_slash =
	       new Quad(*vbuff1, 8, 6, 9, 7, 10);

	ui_smiley = new Quad(*vbuff1, 8, 6, 13, 8, 14);

	ui_exclam = new Quad(*vbuff1, 8, 6, 3, 8, 4);

	ui_title = new Quad(*vbuff1, 8, 0, -1, 8, 6);

	cam1 = new Camera();

	ortho1 = new Camera();

	image_cell = new Image();
	image_cell->LoadImage("../Data/cell.webp");
	image_cell->SetSmooth();

	image_marble = new Image();
	image_marble->LoadImage("../Data/marble.webp");
	image_marble->SetSmooth();

	image_brick = new Image();
	image_brick->LoadImage("../Data/bricks.webp");
	image_brick->SetSmooth();

	image_gold = new Image();
	image_gold->LoadImage("../Data/gold.webp");
	image_gold->SetSmooth();

	image_cash = new Image();
	image_cash->LoadImage("../Data/cash.webp");
	image_cash->SetSmooth();

	image_texts = new Image();
	image_texts->LoadImage("../Data/texts.webp");
	image_texts->SetSmooth();

	image_title = new Image();
	image_title->LoadImage("../Data/title.webp");
	image_title->SetSmooth();

	image_playerskin = new Image();
	image_playerskin->LoadImage("../Data/playerskin.webp");
	image_playerskin->SetSmooth();

	image_enemyskin = new Image();
	image_enemyskin->LoadImage("../Data/enemyskin.webp");
	image_enemyskin->SetSmooth();


	cube1 = new ObjPrim(*vbuff1, "../Data/cube.obj", image_cell, 0.5f);
	PrimMap["cube1"] = cube1;

	player1 = new ObjPrim(*vbuff1, "../Data/player.obj", image_playerskin, 0.75f);
	PrimMap["player"] = player1;

	enemy1 = new ObjPrim(*vbuff1, "../Data/enemy.obj", image_enemyskin, 0.75f);
	PrimMap["enemy"] = enemy1;

	light1 = new ObjPrim(*vbuff1, "../Data/cube.obj", image_cell, 0.1f);
	PrimMap["light1"] = light1;

	floor1 = new ObjPrim(*vbuff1, "../Data/floor.obj", image_marble);
	PrimMap["floor1"] = floor1;

	wall1 = new ObjPrim(*vbuff1, "../Data/wall.obj", image_brick);
	PrimMap["wall1"] = wall1;


	loot1 = new ObjPrim(*vbuff1, "../Data/goldbars.obj", image_gold);
	PrimMap["goldbars"] = loot1;

	loot2 = new ObjPrim(*vbuff1, "../Data/cash.obj", image_cash, 0.75f);
	PrimMap["cash"] = loot2;

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
	delete vbuff1;

	delete q1;

	delete ui_loot;
	delete ui_win;
	delete ui_caught;
	delete ui_1;
	delete ui_2;
	delete ui_3;
	delete ui_4;
	delete ui_5;
	delete ui_6;
	delete ui_7;
	delete ui_8;
	delete ui_9;
	delete ui_0;
	delete ui_slash;

	delete ui_smiley;
	delete ui_exclam;
	delete ui_title;

	delete image_cell;
	delete image_marble;
	delete image_brick;
	delete image_gold;
	delete image_cash;
	delete image_texts;
	delete image_title;

	delete image_playerskin;
	delete image_enemyskin;

	delete cube1;

	delete player1;

	delete enemy1;

	delete light1;
	delete floor1;
	delete wall1;

	delete loot1;
	delete loot2;

	delete cam1;
	delete ortho1;
}

void Game::Resize(int w, int h)
{
	LOGf("Resize: %dx%d", w, h);

	glViewport(0,0, w, h);
	cam1->Resize(w, h);

	ortho1->projection_matrix = glm::ortho(0.0f, float(w), 0.0f, float(h));
	ortho1->view_matrix = mat4();

	ortho1->projection_view_matrix = ortho1->projection_matrix * ortho1->view_matrix;

}

bool control_cam_up = false;
bool control_cam_down = false;

bool show_lights = false;  ///< debug light positions with small cubes
bool level_loaded = false;
bool sort_objects = true;  ///< depth sort front to back, to minimize frag shader runs
bool show_title = true;

void Game::Update(float dt)
{
	if (not level_loaded)
	{
		ParseLevel("../Data/level1.txt");
		level_loaded=true;

		ASSERT(the_player);

	}


	the_player->Update(dt);

	for (auto e : Enemies)
	{
		e->Update(dt);
	}

	for (auto e : entities)
	{
		e->Update(dt);
	}

	float camspeed = 8.0f;

	cam1->look_at = the_player->player->position;
	cam1->position_set = the_player->player->position + cam_offset;

	vec3 cam_vel = glm::vec3();

	if (control_cam_up) cam_vel.y += 1.0f;
	if (control_cam_down) cam_vel.y -= 1.0f;

	cam1->position.y = cam1->position.y + ( cam_vel.y * camspeed * dt);

	float camfollowspeed = 2.25f;

	cam1->position.x = glm::mix(cam1->position.x, cam1->position_set.x, dt * camfollowspeed);
	cam1->position.z = glm::mix(cam1->position.z, cam1->position_set.z, dt * camfollowspeed);


	cam1->Update(dt);


	if (current_score >= max_score)
	{
		if (not level_won) PlaySound("win");

		level_won = true;
	}

}



void Game::Key(SDL_Keycode key, bool keydown)
{
	if (keydown) show_title = false;

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

	if (keydown and key == SDLK_F1)
	{
		show_title = true;
	}

	if (keydown and ((key == SDLK_F5) or ((level_won or level_lost) and (key == SDLK_SPACE))))
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


	for (auto e: entities)
	{
		SetLights(prog2, e->position);

		e->Render(cam1, prog2);
	}


	//prog2->SetTexture(image_cell);
	for (auto e: Enemies)
	{
		SetLights(prog2, e->player->position);

		e->player->Render(cam1, prog2);
	}


	RenderGUI();
}

void Game::RenderText(float offx, float offy, const string &txt)
{
	mat4 translate_matrix = glm::translate(mat4(), vec3(offx, offy, 0.0f));

	for (unsigned i = 0; i < txt.size(); ++i)
	{
		char ch = txt[i];
		prog1->SetModel(translate_matrix);

		if (ch == '1') ui_1->Draw();
		if (ch == '2') ui_2->Draw();
		if (ch == '3') ui_3->Draw();
		if (ch == '4') ui_4->Draw();
		if (ch == '5') ui_5->Draw();
		if (ch == '6') ui_6->Draw();
		if (ch == '7') ui_7->Draw();
		if (ch == '8') ui_8->Draw();
		if (ch == '9') ui_9->Draw();
		if (ch == '0') ui_0->Draw();
		if (ch == '/') ui_slash->Draw();

		translate_matrix = glm::translate(translate_matrix, vec3(32.0, 0.0f, 0.0f));
	}
}

void Game::RenderGUI()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	//glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	prog1->Use(vbuff1);
	prog1->SetTexture(image_texts);

	//check for speech bubbles

	prog1->SetCamera(cam1);

	if (the_player->emote_time > 0.0f )
	{
		vec3 pos = the_player->player->position;
		mat4 model_matrix = glm::translate(mat4(), vec3(pos.x + 0.5f, pos.y + 3.0f, pos.z));
		model_matrix = glm::scale(model_matrix, vec3(0.02f));

		prog1->SetModel(model_matrix);
		ui_smiley->Draw();
		//LOG(":D");
	}

	for (auto e : Enemies)
	{
		if (e->emote_time > 0.0f )
		{
			vec3 pos = e->player->position;
			mat4 model_matrix = glm::translate(mat4(), vec3(pos.x + 0.5f, pos.y + 3.0f, pos.z));
			model_matrix = glm::scale(model_matrix, vec3(0.02f));

			prog1->SetModel(model_matrix);
			ui_exclam->Draw();
		}
	}


	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	prog1->SetCamera(ortho1);

	//mat4 scale_matrix = glm::scale(mat4(), vec3(10.0f));

	mat4 translate_matrix = glm::translate(mat4(), vec3(80.0f, 60.0f, 0.0f));

	mat4 model_matrix = translate_matrix ;

	prog1->SetModel(model_matrix);



	ui_loot->Draw();

	stringstream score_txt;
	score_txt << current_score << "/" << max_score;

	RenderText(180.0f, 60.0f, score_txt.str());


	if (level_won)
	{
		mat4 translate_matrix = glm::translate(mat4(), vec3(WIDTH/2.0f, HEIGHT/2.0f, 0.0f));
		mat4 scale_matrix = glm::scale(translate_matrix, vec3(4.0f));
		prog1->SetModel(scale_matrix);

		ui_win->Draw();
	}

	if (level_lost)
	{
		mat4 translate_matrix = glm::translate(mat4(), vec3(WIDTH/2.0f, HEIGHT/2.0f, 0.0f));
		mat4 scale_matrix = glm::scale(translate_matrix, vec3(4.0f));
		mat4 model_matrix = glm::rotate(scale_matrix, 30.0f, vec3(0.0f, 0.0f, 1.0f));

		prog1->SetModel(model_matrix);

		ui_caught->Draw();

	}

	if (show_title)
	{
		mat4 translate_matrix = glm::translate(mat4(), vec3(WIDTH/2.0f, HEIGHT/2.0f, 0.0f));
		mat4 scale_matrix = glm::scale(translate_matrix, vec3(2.0f));
		prog1->SetModel(scale_matrix);

		prog1->SetTexture(image_title);

		ui_title->Draw();

	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


}
