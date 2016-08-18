#include "vld.h"

#include "KapokServer.h"

#include "SDL2/SDL.h"
#ifdef main
#undef main
#endif // main

int main()
{
	{//Init SDL
		/*auto ret =*/ SDL_Init(SDL_INIT_EVENTS);
		auto window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
		/*auto renderer =*/ SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	}

	KapokServer server;

	server.Start();

	{//SDL
		SDL_Event evt;
		auto quit = false;
		while ( !quit )
		{
			while ( SDL_WaitEvent(&evt) )
			{
				if ( (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)
					|| evt.type == SDL_QUIT )
				{
					quit = true;
					break;
				}
			}
		}

		SDL_Quit();
	}

	server.Stop();

	server.Join();
}