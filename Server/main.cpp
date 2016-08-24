#include "vld.h"

#include "KapokServer.h"
#include "DebugSink.h"

#include "SDL2/SDL.h"

#include "TestUnit/ServerTestUnit.h"

#include <boost/program_options.hpp>

int main(int argc, char **argv)
{
	DebugSink dbgSink;
	
	{
		using namespace boost::program_options;

		options_description desc;
		desc.add_options()
			( "test,t", "Run Test Unit" );

		variables_map vm;
		store( parse_command_line( argc, argv, desc ), vm );
		notify( vm );

		if ( vm.count( "test" ) )
		{
			ServerTestUnit::Run();
			return 0;
		}
	}
	
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

	return 0;
}