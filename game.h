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

	Camera *ortho1 = nullptr;


	Quad *q1 = nullptr;

	Quad *ui_loot = nullptr;
	Quad *ui_win = nullptr;
	Quad *ui_caught = nullptr;
	Quad *ui_1 = nullptr;
	Quad *ui_2 = nullptr;
	Quad *ui_3 = nullptr;
	Quad *ui_4 = nullptr;
	Quad *ui_5 = nullptr;
	Quad *ui_6 = nullptr;
	Quad *ui_7 = nullptr;
	Quad *ui_8 = nullptr;
	Quad *ui_9 = nullptr;
	Quad *ui_0 = nullptr;
	Quad *ui_slash = nullptr;


	Image *image_cell = nullptr;
	Image *image_marble = nullptr;
	Image *image_brick = nullptr;
	Image *image_gold = nullptr;
	Image *image_cash = nullptr;

	Image *image_texts = nullptr;

	ObjPrim *cube1 = nullptr;

	ObjPrim *player1 = nullptr;

	ObjPrim *enemy1 = nullptr;

	ObjPrim *floor1 = nullptr;

	ObjPrim *wall1 = nullptr;

	ObjPrim *light1 = nullptr;


	ObjPrim *loot1 = nullptr;
	ObjPrim *loot2 = nullptr;


	void InitGL();

	void DestroyGL();

	void Update(float dt);

	void Resize(int w, int h);

	void Render();

	void RenderGUI();
	void RenderText(float offx, float offy, const string &txt);

	void RenderWorld();

	void Key(SDL_Keycode key, bool down);

};

#endif /* GAME_H_ */
