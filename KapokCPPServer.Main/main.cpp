#include "vld.h"

#include "AsyncThreadPool.h"

#include "TCPServer.h"
#include "TcpSession.h"

#include "WSServer.h"
#include "WSSession.h"

#include "SDL2/SDL.h"

#include <vector>
#include <iostream>

#ifdef main
#undef main
#endif // main

int main()
{
	{//Init SDL
		auto ret = SDL_Init(SDL_INIT_EVENTS);
		auto window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
		auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	}

	//{TCPServer
	AsyncThreadPool tcpThreadPool;
	tcpThreadPool.Start(1);

	TCPServer tcpServer(tcpThreadPool.GetIOService());

	std::vector<TcpSessionSPtr> tcpsessionList;

	tcpServer.GetListener().OnAccept_.connect([&tcpsessionList](TcpSessionSPtr& session)
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

		tcpsessionList.push_back(session);
	});

	tcpServer.StartAccept( 6001 );
	//}

	//{WSServer
	AsyncThreadPool wsThreadPool;
	wsThreadPool.Start(1);

	WSServer wsServer(wsThreadPool.GetIOService());
	std::vector<WSSessionSPtr> wssessionList;

	static beast::websocket::opcode op;
	static beast::streambuf buf;

	wsServer.GetListener().OnAccept_.connect([&](WSSessionSPtr& session)
	{
		
		session->GetStream().async_read(op, buf, [session](const auto& ec)
		{
			if ( ec )
			{
				auto i = 0;
			}

			
			session->GetStream().write(boost::asio::buffer("Hello"));
		});
		wssessionList.push_back(session);

		auto i = 0;
	});

	wsServer.StartAccept(6002);
	//}


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

	tcpServer.StopAccept();
	tcpThreadPool.Stop();
	tcpThreadPool.Join();

	wsServer.StopAccept();
	wsThreadPool.Stop();
	wsThreadPool.Join();
}