#include "KapokServer.h"

#include "AsyncThreadPool.h"
#include "TCPServer.h"
#include "TcpSession.h"
#include "WSServer.h"
#include "WSSession.h"

#include <iostream>
#include <vector>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
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
	};

public:

	SConfig		ConfigInfo_;

	AsyncThreadPool	TcpThreadPool_;
	TCPServer		TcpServer_{ TcpThreadPool_.GetIOService() };
	std::vector<TcpSessionSPtr>	TcpSessionList_;
	std::mutex		TcpMutex_;
	
	AsyncThreadPool	WebsocketThreadPool_;
	WSServer		WebsocketServer_{ WebsocketThreadPool_.GetIOService() };
	std::vector<WSSessionSPtr>	WSSessionList_;
	std::mutex		WSMutex_;

	AsyncThreadPool	LogicThreadPool_;

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
		WebsocketServer_.GetListener().OnAccept_.connect([this](WSSessionSPtr& session)
		{
			session->GetListener().OnMessage_.connect([this](const beast::websocket::opcode& op, WSSessionSPtr& session)
			{
				LogicThreadPool_.Post([this, op, sessionPtr = std::move(session)]() mutable
				{
					DispatchMsg(op, sessionPtr);
				});
			});

			session->Receive();

			{
				std::unique_lock<decltype(TcpMutex_)> lock(TcpMutex_);
				WSSessionList_.push_back(std::move(session));
			}
		});

		WebsocketThreadPool_.Start(ConfigInfo_.WebsocketThread_);
		WebsocketServer_.StartAccept(ConfigInfo_.WebsocketPort);
	}

	void	DispatchMsg(const beast::websocket::opcode& op, WSSessionSPtr& session)
	{
		auto& recvBuf = session->GetRecvBuf();

		std::string s;
		{
			auto cb = beast::consumed_buffers(recvBuf.data(), 0);
			s.reserve(boost::asio::buffer_size(cb));
			for (auto const& buffer : cb)
			{
				s.append(boost::asio::buffer_cast<const char*>(buffer), boost::asio::buffer_size(buffer));
			}
		}

		recvBuf.consume(recvBuf.size());

		try
		{
			std::istringstream is(s);
			boost::property_tree::ptree pt;
			boost::property_tree::json_parser::read_json(is, pt);

			auto messageName = pt.get_optional<std::string>("MessageName");
			if (!messageName)
			{
				return;
			}

			if (*messageName == "RenderRequest")
			{

			}
		}
		catch (const std::exception&)
		{

		}
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

	try
	{
		imp_.ReadConfig();

		imp_.StartTCPServer();

		imp_.StartWebsocketServer();

		imp_.LogicThreadPool_.Start(1);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

void KapokServer::Stop()
{
	auto& imp_ = *ImpUPtr_;

	imp_.LogicThreadPool_.Stop();

	imp_.TcpServer_.StopAccept();
	imp_.TcpThreadPool_.Stop();

	imp_.WebsocketServer_.StopAccept();
	imp_.WebsocketThreadPool_.Stop();
}

void KapokServer::Join()
{
	auto& imp_ = *ImpUPtr_;

	ImpUPtr_->LogicThreadPool_.Join();
	imp_.TcpThreadPool_.Join();
	imp_.WebsocketThreadPool_.Join();
}