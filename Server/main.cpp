#include "vld.h"

#include <windows.h>

#include "KapokServer.h"
#include "DebugSink.h"

#include "SDL2/SDL.h"
#ifdef main
#undef main
#endif // main

#include "TestUnit/ServerTestUnit.h"

#include <boost/program_options.hpp>

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{
	DebugSink dbgSink;
	
	{
		using namespace boost::program_options;

		LPWSTR *szArgList;
		int argCount;
		szArgList = CommandLineToArgvW( GetCommandLineW(), &argCount );

		options_description desc;
		desc.add_options()
			( "test,t", "Run Test Unit" );

		variables_map vm;
		store( parse_command_line( argCount, szArgList, desc ), vm );
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