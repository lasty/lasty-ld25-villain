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
#include <list>

#include "globals.h"

#include "os_utils.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_audio.h"

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


struct Wave
{
	Uint32 length;
	Uint8 *buffer;
	SDL_AudioSpec spec;

	Wave(string filename)
	{
		//string data = ReadFile(filename);
		//ASSERT(data.size());

		if (SDL_LoadWAV( filename.c_str(), &spec, &buffer, &length) == nullptr)
		{
			LOGf("%s ", SDL_GetError());

			THROW("Could not load wav file");
		}
	}

	virtual ~Wave()
	{
		SDL_FreeWAV(buffer);
	}
};


struct Channel
{
	Wave *waveref;
	float volume;

	Uint8 *audio_pos;
	int audio_len;

	bool done = false;

	Channel(Wave* sample, float volume)
	:waveref(sample), volume(volume), audio_pos(sample->buffer), audio_len(sample->length)
	{

	}

	void Mix(Uint8* stream, int len)
	{
		if (audio_len <= 0)
		{
			done = true;
			return;
		}
		len = len > audio_len ? audio_len : len;

		SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME * volume);

		audio_pos += len;
		audio_len -= len;
	}
};

std::list<Channel> Mixer;

class Audio
{
public:
	Audio()
	{
		SDL_AudioSpec wav_spec;

		wav_spec.freq = 44100;
		wav_spec.format = AUDIO_U16SYS;
		wav_spec.channels = 1;
		wav_spec.callback = audio_callback;
		wav_spec.userdata = nullptr;
		wav_spec.samples = 512;

		SDL_AudioSpec got;

		SDL_Init(SDL_INIT_AUDIO);

		if ( SDL_OpenAudio(&wav_spec, &got) < 0) {
			LOGf("%s", SDL_GetError());
			THROW("Could not open audio");
		}

		SDL_PauseAudio(0);

	}

	virtual ~Audio()
	{
		SDL_PauseAudio(1);

		SDL_CloseAudio();
	}

	void PlaySound(Wave *sample, float volume = 1.0f)
	{
		SDL_LockAudio();
		Mixer.push_back(Channel(sample, volume));
		SDL_UnlockAudio();
	}

	static void audio_callback(void *unused, Uint8 *stream, int len)
	{
		memset(stream, 0, len);

		for( auto &chan : Mixer )
		{
			chan.Mix(stream, len);
		}

		for(auto i = Mixer.begin(); i != Mixer.end(); )
		{
			if (i->done)
			{
				Mixer.erase(i);
				i = Mixer.begin();
			}
			else
			{
				i++;
			}
		}
	}
};


Wave *w1 = nullptr;
Wave *w2 = nullptr;
Wave *w3 = nullptr;

Audio *audio = nullptr;

void InitAudio()
{
	audio = new Audio();

	w1 = new Wave("../Data/win.wav");
	//w1 = new Wave("../Data/pickup_coin.wav");
	w2 = new Wave("../Data/lose.wav");
	w3 = new Wave("../Data/coin.wav");
}

void PlaySound(std::string which, float volume)
{
	if (not audio) return;

	Wave *w = nullptr;

	if (which == "win") w = w1;
	if (which == "lose") w = w2;
	if (which == "loot") w = w3;

	if (w)
	audio->PlaySound(w, volume);
}

void DeleteAudio()
{
	delete audio;

	delete w1;
	delete w2;
	delete w3;

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

		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

		InitAudio();

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

		DeleteAudio();

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
