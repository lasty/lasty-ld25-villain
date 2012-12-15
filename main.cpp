/*
 * main.cpp
 *
 *  For Ludum Dare 25:  You are the Villain
 *
 *  Created on: 15/12/2012
 *      Author: Lasty
 */

#include <iostream>
#include <sstream>

#include "globals.h"

#include "os_utils.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include "SDL.h"
#include "SDL_events.h"

//sdl in windows redefines main
#undef main

#include "GL/glee.h"
#include "GL/glu.h"

void GLCheckError(const char* file, int line)
{
	int err = glGetError();
	if (err == GL_NO_ERROR) return;

	static int errcount = 0;

	const GLubyte* errstr = gluErrorString(err);

	LOGfRaw("GLError #%d: %s  [%s : %d]", errcount, errstr, file, line);

	if (++errcount > 5) THROW("Error count too high");
}

#include "game.h"

///globals
const string TITLE = ("lasty_ld25_villain");
int WIDTH = 800;
int HEIGHT = 600;

bool running = true;

Game * the_game;

void ProcessEvent(SDL_Event &e)
{
	switch (e.type)
	{
		case SDL_QUIT:
			running = false;
		break;

		case SDL_WINDOWEVENT:
		{
			switch (e.window.event)
			{
				case SDL_WINDOWEVENT_RESIZED:
				{
					WIDTH = e.window.data1;
					HEIGHT = e.window.data2;
					the_game->Resize(WIDTH, HEIGHT);
				}
			}
		}
		break;

		case SDL_KEYDOWN:
			the_game->Key(e.key.keysym.sym, true);
		break;

		case SDL_KEYUP:
			the_game->Key(e.key.keysym.sym, false);
		break;

	}
}

int main(int argc, char* argv[])
{
	try
	{
		ChangeToExecutableDirectory(argc, argv);

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

		auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

		SDL_Window *window = SDL_CreateWindow(TITLE.c_str(), SDL_WINDOWPOS_CENTERED, 50, WIDTH, HEIGHT, flags);

		SDL_GLContext context = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window, context);

		SDL_GL_SetSwapInterval(0);  //disable vsync

		SDL_ShowWindow(window);

		SDL_Event e;

		long lasttime = SDL_GetTicks();

		the_game = new Game();

		the_game->InitGL();
		the_game->Resize(WIDTH, HEIGHT);

		while (running)
		{
			while (SDL_PollEvent(&e))
			{

				ProcessEvent(e);
			}

			float thistime = SDL_GetTicks();
			float dt = (thistime - lasttime) / 1000.0f;
			lasttime = thistime;
			the_game->Update(dt);

			the_game->Render();
			SDL_GL_SwapWindow(window);
		}

		SDL_HideWindow(window);

		the_game->DestroyGL();

		delete the_game;

		SDL_GL_DeleteContext(context);

		SDL_DestroyWindow(window);

	}
	catch (Exception &e)
	{
		std::stringstream what;
		what << "Exception:  " << e.what << "    [" << e.file << " : " << e.line << "]";
		PopupErrorBox(what.str());
	}

	return 0;
}
