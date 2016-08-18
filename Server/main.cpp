#include "vld.h"

#include "AsyncThreadPool.h"

#include "TCPServer.h"
#include "TcpSession.h"

#include "WSServer.h"
#include "WSSession.h"

#include "SDL2/SDL.h"

#include <vector>
#include <iostream>

#include <boost/asio.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#ifdef main
#undef main
#endif // main

int main()
{
	auto tcpPort = 6001U, tcpThread = 1U;
	auto wsPort = 6002U, wsThread = 1U;
	{
		try
		{
			boost::filesystem::path configXML("config.xml");
			if (boost::filesystem::exists(configXML))
			{
				boost::property_tree::ptree pt;
				boost::filesystem::fstream configf(configXML);
				auto flags = boost::property_tree::xml_parser::no_comments | boost::property_tree::xml_parser::trim_whitespace;
				boost::property_tree::read_xml(configf, pt, flags);

				auto tmptcpPort = pt.get<decltype(tcpPort)>("TCPServer.<xmlattr>.Port");
				auto tmptcpThread = pt.get<decltype(tcpThread)>("TCPServer.<xmlattr>.Thread");
				auto tmpwsPort = pt.get<decltype(wsPort)>("WebsocketServer.<xmlattr>.Port");
				auto tmpwsThread = pt.get<decltype(wsThread)>("WebsocketServer.<xmlattr>.Thread");

				tcpPort = tmptcpPort;
				tcpThread = tmptcpThread;
				wsPort = tmpwsPort;
				wsThread = tmpwsThread;
			}
		}
		catch (...)
		{

		}
		
	}

	{//Init SDL
		/*auto ret =*/ SDL_Init(SDL_INIT_EVENTS);
		auto window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
		/*auto renderer =*/ SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	}

	//{TCPServer
	AsyncThreadPool tcpThreadPool;
	tcpThreadPool.Start(tcpThread);

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

	tcpServer.StartAccept( tcpPort );
	//}

	//{WSServer
	AsyncThreadPool wsThreadPool;
	wsThreadPool.Start(wsThread);

	WSServer wsServer(wsThreadPool.GetIOService());
	std::vector<WSSessionSPtr> wssessionList;

	static beast::websocket::opcode op;
	static beast::streambuf buf;

	wsServer.GetListener().OnAccept_.connect([&](WSSessionSPtr& session)
	{
		
		session->GetListener().OnMessage_.connect([](const beast::websocket::opcode& op, WSSessionSPtr& session)
		{
			auto& recvBuf = session->GetRecvBuf();

			auto cb = beast::consumed_buffers(recvBuf.data(), 0);

			std::string s;
			s.reserve(boost::asio::buffer_size(cb));

			for (auto const& buffer : cb)
			{
				s.append(boost::asio::buffer_cast<const char*>(buffer), boost::asio::buffer_size(buffer));
			}

			std::cout << s << std::endl;

			session->Send(boost::asio::buffer(s + " call back"), op);
		});

		session->GetListener().OnPostSend_.connect([](const ErrCode& ec, WSSessionSPtr& session)
		{
			if (ec)
			{
				session->Close();
			}
			else
			{
				session->Receive();
			}
		});

		session->Receive();

		wssessionList.push_back(session);

		auto i = 0;
	});

	wsServer.StartAccept(wsPort);
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