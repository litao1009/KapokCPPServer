#include "WSPPServer.h"
#include "WSPPSession.h"

#include "websocketpp/server.hpp"
#include "websocketpp/config/asio_no_tls.hpp"

class	WSPPServer::Imp
{
public:

	using	ServerType = websocketpp::server<websocketpp::config::asio>;

public:

	Imp(IOService& ios) :IOService_(ios){}

	IOService&	IOService_;
	ServerType	WSServer_;
	Listener	Listener_;
	WSPPServer*	ThisPtr_{};

public:

	void	Init()
	{
		WSServer_.set_access_channels(websocketpp::log::alevel::none);

		WSServer_.set_open_handler([thisPtr = ThisPtr_](websocketpp::connection_hdl connHDL)
		{
			auto& imp_ = *(thisPtr->ImpUPtr_);
			std::error_code ec;
			auto conn = imp_.WSServer_.get_con_from_hdl(connHDL, ec);

			auto session = std::make_shared<WSPPSession>(connHDL);
			WSPPSessionWPtr sessionWPtr = session;

			conn->set_open_handler([sessionWPtr](websocketpp::connection_hdl connHDL)
			{
				auto sptr = sessionWPtr.lock();
				sptr->GetListener().OnOpen_(sptr);
			});

			conn->set_close_handler([sessionWPtr](websocketpp::connection_hdl connHDL)
			{
				auto sptr = sessionWPtr.lock();
				sptr->GetListener().OnClose_(sptr);
			});

			conn->set_message_handler([sessionWPtr](websocketpp::connection_hdl hdl, Imp::ServerType::message_ptr msg)
			{
				auto sptr = sessionWPtr.lock();
				sptr->GetListener().OnMessage_(sptr, msg);
			});

			conn->set_send_handler([sessionWPtr](websocketpp::connection_hdl connHDL)
			{
				auto sptr = sessionWPtr.lock();
				sptr->GetListener().OnPostSend_(sptr);
			});

			imp_.Listener_.OnAccept_(session);
		});

		WSServer_.init_asio(&IOService_);

// 		WSServer_.set_close_handler([this](websocketpp::connection_hdl sessionHDL)
// 		{
// 
// 		});
// 
// 		WSServer_.set_message_handler([this](websocketpp::connection_hdl hdl, Imp::ServerType::message_ptr msg)
// 		{
// 
// 		});
	}
};

WSPPServer::WSPPServer(IOService& ios):ImpUPtr_(std::make_unique<Imp>(ios))
{
	auto& imp_ = *ImpUPtr_;
	imp_.ThisPtr_ = this;
	imp_.Init();
}

WSPPServer::~WSPPServer()
{
	
}

WSPPServer::WSPPServer(WSPPServer && rhs):ImpUPtr_(std::move(rhs.ImpUPtr_))
{
}

ErrCode WSPPServer::StartAccept(uint16_t listenPort)
{
	auto& imp_ = *ImpUPtr_;

	std::error_code ec;
	imp_.WSServer_.listen(listenPort, ec);
	assert(!ec);

	imp_.WSServer_.start_accept(ec);
	assert(!ec);

	return {};
}

ErrCode WSPPServer::StopAccept()
{
	auto& imp_ = *ImpUPtr_;

	std::error_code ec;
	imp_.WSServer_.stop_listening(ec);
	assert(!ec);

	return {};
}

WSPPServer::Listener& WSPPServer::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}