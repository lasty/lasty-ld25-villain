/*
 * game.h
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#ifndef GAME_H_
#define GAME_H_

#include "globals.h"
#include "vertex.h"
#include "program.h"
#include "camera.h"
#include "prim.h"
#include "image.h"
#include "obj.h"

#include "SDL_events.h"

class Game
{
public:
	Game();
	virtual ~Game();

	VertexArray *vbuff1 = nullptr;

	Program *prog1 = nullptr;
	ProgramLighting *prog2 = nullptr;

	Camera *cam1 = nullptr;


	Quad *q1 = nullptr;

	Image *image1 = nullptr;
	Image *image2 = nullptr;

	ObjPrim *cube1 = nullptr;

	ObjPrim *floor1 = nullptr;

	ObjPrim *light1 = nullptr;



	void InitGL();

	void DestroyGL();

	void Update(float dt);

	void Resize(int w, int h);

	void Render();

	void Key(SDL_Keycode key, bool down);

};

#endif /* GAME_H_ */
