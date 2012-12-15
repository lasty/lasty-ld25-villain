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


#include "vertex.h"
#include "program.h"
#include "camera.h"
#include "prim.h"
#include "image.h"


///globals
const string TITLE = ("lasty_ld25_villain");
int WIDTH = 800;
int HEIGHT = 600;


/// app specific
bool running = true;


VertexArray *vbuff1 = nullptr;
Program *prog1 = nullptr;
Camera *cam1 = nullptr;
Quad *q1 = nullptr;

Image *image1 = nullptr;


void Resize(int w, int h)
{
	LOGf("Resize: %dx%d", w, h);

	glViewport(0,0, w, h);
	cam1->Resize(w, h);
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

	vbuff1 = new VertexArray();

	prog1 = new Program("../Data/prog1.glsl");
	q1 = new Quad(*vbuff1, 1.0f);
	cam1 = new Camera();

	image1 = new Image();
	image1->LoadImage("../Data/cell.webp");
}

void DestroyGL()
{
	delete prog1;
	delete q1;
	delete vbuff1;

	delete image1;

	delete cam1;
}


void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	prog1->Use(vbuff1);
	prog1->SetCamera(cam1);
	prog1->SetModel(glm::mat4());
	prog1->SetTexture(image1);
	q1->Draw();

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
