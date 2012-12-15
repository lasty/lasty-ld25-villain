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



const string TITLE = ("lasty_ld25_villain");
int WIDTH = 800;
int HEIGHT = 600;

bool running = true;


void Resize(int w, int h)
{
	LOGf("Resize: %dx%d", w, h);
}

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
					Resize(WIDTH, HEIGHT);
				}
			}
		}
		break;
	}
}


void Update(float dt)
{


}

void InitGL()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

}

void DestroyGL()
{

}


void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

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


		InitGL();
		Resize(WIDTH, HEIGHT);

		while (running)
		{
			while (SDL_PollEvent(&e))
			{

				ProcessEvent(e);
			}

			float thistime = SDL_GetTicks();
			float dt = (thistime - lasttime) / 1000.0f;
			lasttime = thistime;
			Update(dt);


			Render();
			SDL_GL_SwapWindow(window);
		}

		SDL_HideWindow(window);

		DestroyGL();

		SDL_GL_DeleteContext(context);

		SDL_DestroyWindow(window);

	}
	catch (Exception &e)
	{
		std::stringstream what;
		what << "Exception:  " << e.what << "    ["<< e.file << " : " << e.line << "]";
		PopupErrorBox(what.str());
	}

	return 0;
}
