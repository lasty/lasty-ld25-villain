/*
 * game.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include "game.h"

#include <vector>
#include <algorithm>
using std::vector;


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
		prog->SetModel(model_matrix);
		model->Draw();
	}
};


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

Object* player;
glm::vec3 player_vel;

Game::Game()
{

}



Game::~Game()
{

}



void Game::InitGL()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	glEnable(GL_DEPTH_TEST);

	vbuff1 = new VertexArray();

	prog1 = new Program("../Data/prog1.glsl");
	prog2 = new ProgramLighting("../Data/prog_lighting.glsl");

	q1 = new Quad(*vbuff1, 1.0f);

	cam1 = new Camera();

	image1 = new Image();
	image1->LoadImage("../Data/cell.webp");
	image1->SetSmooth();

	image2 = new Image();
	image2->LoadImage("../Data/marble.webp");
	image2->SetSmooth();

	cube1 = new ObjPrim(*vbuff1, "../Data/cube.obj", 0.5f);

	light1 = new ObjPrim(*vbuff1, "../Data/cube.obj", 0.2f);

	floor1 = new ObjPrim(*vbuff1, "../Data/floor.obj");

	cam1->position += vec3(0, 2, 0);
	cam1->CalcMatrixes();

	for (int x=-5; x<15; ++x)
	{
		for( int y=-4; y<14; ++y)
		{
			objs.push_back( new Object(vec3(x*2.0f,0,y*2.0f), vec3(0,0,0), floor1));
		}
	}

	lights.push_back( new Light(vec3(0.0, 1.0f, 0.0f), colour_light_yellow * 5.0f));

	lights.push_back( new Light(vec3(5.0, 1.0f, 8.0f), colour_light_red * 5.0f));

	lights.push_back( new Light(vec3(8.0, 1.0f, 1.0f), colour_light_cyan * 5.0f));


	player = new Object(vec3(0,0,0), vec3(0,0,0), cube1);
}

void Game::DestroyGL()
{
	for(auto i : objs)
	{
		delete i;
	}
	objs.clear();

	delete prog1;
	delete prog2;
	delete q1;
	delete vbuff1;

	delete image1;
	delete image2;

	delete cube1;
	delete floor1;

	delete cam1;
}

void Game::Resize(int w, int h)
{
	LOGf("Resize: %dx%d", w, h);

	glViewport(0,0, w, h);
	cam1->Resize(w, h);
}

bool control_fwd = false;
bool control_back = false;
bool control_left = false;
bool control_right = false;

bool control_cam_up = false;
bool control_cam_down = false;

void Game::Update(float dt)
{
	float playerspeed = 4.0f;
	float camspeed = 8.0f;

	player_vel = glm::vec3();

	if (control_fwd) player_vel.z -= 1.0f;
	if (control_back) player_vel.z += 1.0f;

	if (control_left) player_vel.x -= 1.0f;
	if (control_right) player_vel.x += 1.0f;

	player->position = player->position + ( player_vel * playerspeed * dt);
	player->UpdateMatrixes();

	cam1->look_at = player->position;

	vec3 cam_vel = glm::vec3();

	if (control_cam_up) cam_vel.y += 1.0f;
	if (control_cam_down) cam_vel.y -= 1.0f;

	cam1->position = cam1->position + ( cam_vel * camspeed * dt);

	cam1->Update(dt);

}



void Game::Key(SDL_Keycode key, bool keydown)
{

	if (key == SDLK_w)
	{
		control_fwd = keydown;
	}

	if (key == SDLK_s)
	{
		control_back = keydown;
	}

	if (key == SDLK_a)
	{
		control_left = keydown;
	}

	if (key == SDLK_d)
	{
		control_right = keydown;
	}

	if (key == SDLK_UP)
	{
		control_cam_up = keydown;
	}

	if (key == SDLK_DOWN)
	{
		control_cam_down = keydown;
	}

	if (key == SDLK_ESCAPE)
	{
		running = false;
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


void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	prog1->Use(vbuff1);
	prog1->SetCamera(cam1);
	prog1->SetModel(glm::mat4());


	prog1->SetTexture(image2);


	prog2->Use(vbuff1);
	prog2->SetCamera(cam1);
	prog2->SetTexture(image2);

	for (auto i : objs)
	{
		SetLights(prog2, i->position);

		i->Render(cam1, prog2);
	}

	prog2->Use(vbuff1);
	prog2->SetCamera(cam1);

	SetLights(prog2, player->position);

	prog2->SetTexture(image1);
	player->Render(cam1, prog2);


	for(auto l : lights)
	{
		prog1->Use();
		prog1->SetCamera(cam1);

		mat4 light_pos = glm::translate(mat4(), l->position);
		prog1->SetModel(light_pos);
		light1->Draw();
	}
}

