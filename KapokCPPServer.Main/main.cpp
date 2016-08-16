#include "vld.h"

#include "TCPServer.h"
#include "TcpSession.h"
#include "AsyncThreadPool.h"

#include "SDL.h"

#include <vector>
#include <iostream>

#ifdef main
#undef main
#endif // main

int main()
{
	{
		auto ret = SDL_Init(SDL_INIT_EVENTS);
		auto window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
		auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}

	std::vector<TcpSessionSPtr> sessionList;

	AsyncThreadPool threadPool;
	threadPool.Start(1);

	TCPServer tcpserver(threadPool.GetIOService());

	tcpserver.GetListener().OnAccept_.connect([&sessionList](TcpSessionSPtr& session)
	{
		session->GetListener().OnPostReceive_.connect([](TcpSessionSPtr& thisPtr, const ErrCode& ec, TcpSession::BufferType& buf)
		{
			if ( !ec )
			{
				std::cout << buf.data() << std::endl;
				thisPtr->Send(boost::asio::buffer(buf));
			}
		});

		session->GetListener().OnPostSend_.connect([](TcpSessionSPtr& thisPtr, const ErrCode& ec)
		{
			if ( !ec )
			{
				thisPtr->Receive();
			}
		});
		
		session->Receive();

		sessionList.push_back(session);
	});

	tcpserver.StartAccept( 13 );

	{
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

	tcpserver.StopAccept();
	threadPool.Stop();

	threadPool.Join();
}