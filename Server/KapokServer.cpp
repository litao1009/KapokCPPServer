#include "KapokServer.h"

#include "AsyncThreadPool.h"
#include "TCPServer.h"
#include "TcpSession.h"
#include "WSServer.h"
#include "WSSession.h"
#include "WSPPServer.h"
#include "WSPPSession.h"

#include "Processor/IProcessor.h"

#include "google/protobuf/stubs/common.h"

#include <iostream>
#include <vector>

#include <boost/format.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

class	KapokServer::Imp
{
public:

	class	SConfig
	{
	public:

		uint32_t	TcpPort_{6001};
		uint32_t	TcpThread_{ 1 };
		uint32_t	WebsocketPort{ 6002 };
		uint32_t	WebsocketThread_{ 1 };
		uint32_t	ProcessorThread_{ 1 };

	public:

		std::string	Dump()
		{
			using namespace std::string_literals;

			auto div = "=======================Config=========================\n"s;

			boost::format fmt( "%sStart TCP at port %d with %d threads.\nStart Websocket at port %d with %d threads.\nStart %d processor threads.\n%s" );

			fmt % div
				% TcpPort_ % TcpThread_
				% WebsocketPort % WebsocketThread_
				% ProcessorThread_
				% div;

			return fmt.str();
		}
	};

public:

	SConfig		ConfigInfo_;

	AsyncThreadPool	TcpThreadPool_;
	TCPServer		TcpServer_{ TcpThreadPool_.GetIOService() };
	std::vector<TcpSessionSPtr>	TcpSessionList_;
	std::mutex		TcpMutex_;
	
	AsyncThreadPool	WebsocketThreadPool_;
	WSPPServer		WebsocketServer_{ WebsocketThreadPool_.GetIOService() };
	std::vector<WSPPSessionSPtr>	WSSessionList_;
	std::mutex		WSMutex_;

	std::mutex		ConsoleMutex_;

	AsyncThreadPool	ProcThreadPool_;

public:

	void	ReadConfig()
	{
		try
		{
			SConfig tmpConfig;

			boost::filesystem::path configXML("config.xml");
			if (boost::filesystem::exists(configXML))
			{
				boost::property_tree::ptree pt;
				boost::filesystem::fstream configf(configXML);
				auto flags = boost::property_tree::xml_parser::no_comments | boost::property_tree::xml_parser::trim_whitespace;
				boost::property_tree::read_xml(configf, pt, flags);

				tmpConfig.TcpPort_ = pt.get<decltype(ConfigInfo_.TcpPort_)>("TCPServer.<xmlattr>.Port");
				tmpConfig.TcpThread_ = pt.get<decltype(ConfigInfo_.TcpThread_)>("TCPServer.<xmlattr>.Thread");
				tmpConfig.WebsocketPort = pt.get<decltype(ConfigInfo_.WebsocketPort)>("WebsocketServer.<xmlattr>.Port");
				tmpConfig.WebsocketThread_ = pt.get<decltype(ConfigInfo_.WebsocketThread_)>("WebsocketServer.<xmlattr>.Thread");

				ConfigInfo_ = tmpConfig;
			}
		}
		catch (std::exception& e)
		{
			std::cout << "ConfigError:" << e.what() << std::endl;
			std::cout << "Use Default Config." << std::endl;
		}
	}

	void	StartTCPServer()
	{
		TcpServer_.GetListener().OnAccept_.connect([this](TcpSessionSPtr& session)
		{
			session->GetListener().OnPostReceive_.connect([](TcpSessionSPtr& thisPtr, const ErrCode& ec, TcpSession::BufferType& buf)
			{
				if (!ec)
				{
					std::cout << buf.data() << std::endl;
					thisPtr->Send(boost::asio::buffer(buf));
				}
			});

			session->GetListener().OnPostSend_.connect([](TcpSessionSPtr& thisPtr, const ErrCode& ec)
			{
				if (!ec)
				{
					thisPtr->Receive();
				}
			});

			session->Receive();

			{
				std::unique_lock<decltype(TcpMutex_)> lock(TcpMutex_);
				TcpSessionList_.push_back(std::move(session));
			}
			
		});

		TcpThreadPool_.Start(ConfigInfo_.TcpThread_);
		TcpServer_.StartAccept(ConfigInfo_.TcpPort_);
	}

	void	StartWebsocketServer()
	{
		WebsocketServer_.GetListener().OnAccept_.connect([this](auto& session)
		{
			session->GetListener().OnMessage_.connect([this](WSPPSessionSPtr& session, MessagePtr& msg)
			{
				ProcThreadPool_.Post([this, msg, sessionPtr = std::move(session)]() mutable
				{
					IProcessor::DispatchMsg(ProcThreadPool_, msg, sessionPtr);
				});
			});

			session->Receive();

			{
				std::unique_lock<decltype(WSMutex_)> lock(WSMutex_);
				WSSessionList_.push_back(std::move(session));
			}
		});

		WebsocketThreadPool_.Start(ConfigInfo_.WebsocketThread_);
		WebsocketServer_.StartAccept(ConfigInfo_.WebsocketPort);
	}
};

KapokServer::KapokServer():ImpUPtr_(std::make_unique<Imp>())
{

}

KapokServer::~KapokServer()
{

}

void KapokServer::Start()
{
	auto& imp_ = *ImpUPtr_;

	auto& mutex_ = imp_.ConsoleMutex_;
	
	imp_.TcpThreadPool_.GetListener().OnStartThread_.connect([&mutex_]()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		std::cout << "TCP Thread: " << std::this_thread::get_id() << std::endl;
	});

	imp_.WebsocketThreadPool_.GetListener().OnStartThread_.connect([&mutex_]()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		std::cout << "Websocket Thread: " << std::this_thread::get_id() << std::endl;
	});

	imp_.ProcThreadPool_.GetListener().OnStartThread_.connect([&mutex_]()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		std::cout << "Processor Thread: " << std::this_thread::get_id() << std::endl;
	});

	try
	{
		imp_.ReadConfig();

		{
			std::unique_lock<std::mutex> lock( imp_.ConsoleMutex_ );
			std::cout << imp_.ConfigInfo_.Dump() << std::endl;
		}

		imp_.StartTCPServer();

		imp_.StartWebsocketServer();

		imp_.ProcThreadPool_.Start(1);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

void KapokServer::Stop()
{
	auto& imp_ = *ImpUPtr_;

	imp_.ProcThreadPool_.Stop();

	imp_.TcpServer_.StopAccept();
	imp_.TcpThreadPool_.Stop();

	imp_.WebsocketServer_.StopAccept();
	imp_.WebsocketThreadPool_.Stop();

	google::protobuf::ShutdownProtobufLibrary();
}

void KapokServer::Join()
{
	auto& imp_ = *ImpUPtr_;

	ImpUPtr_->ProcThreadPool_.Join();
	imp_.TcpThreadPool_.Join();
	imp_.WebsocketThreadPool_.Join();
}