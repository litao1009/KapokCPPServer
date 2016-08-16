#include "WSServer.h"
#include "WSSession.h"

#include <boost/asio.hpp>

class	WSServer::Imp
{
public:

	Imp(IOService& ios):Acceptor_(ios)
	{}

	boost::asio::ip::tcp::acceptor	Acceptor_;
	Listener						Listener_;

public:

	void	SocketAccept()
	{
		auto session = std::make_shared<WSSession>(Acceptor_.get_io_service());
		auto& sock = session->GetStream().next_layer();
		Acceptor_.async_accept(sock, [this, sessionPtr = std::move(session)](const auto& ec) mutable
		{
			if ( ec )
			{
				Listener_.OnAcceptError_(ec);
			}
			else
			{
				WSAccept(sessionPtr);

				if ( Acceptor_.is_open() )
				{
					SocketAccept();
				}
			}
		});
	}

	void	WSAccept(WSSessionSPtr& session)
	{
		session->GetStream().async_accept([this, session](const auto& ec) mutable
		{
			if ( ec )
			{
				Listener_.OnAcceptError_(ec);
			}
			else
			{
				Listener_.OnAccept_(session);
			}
		});
	}
};

WSServer::WSServer(IOService& ios):ImpUPtr_(std::make_unique<Imp>(ios))
{

}

WSServer::~WSServer()
{

}

ErrCode WSServer::StartAccept(uint16_t listenPort)
{
	ErrCode ec;

	auto& imp_ = *ImpUPtr_;

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), listenPort);

	imp_.Acceptor_.open(ep.protocol(), ec);
	if ( ec )
	{
		return ec;
	}

	imp_.Acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
	if ( ec )
	{
		return ec;
	}

	imp_.Acceptor_.bind(ep, ec);
	if ( ec )
	{
		return ec;
	}

	imp_.Acceptor_.listen(boost::asio::socket_base::max_connections, ec);
	if ( ec )
	{
		return ec;
	}

	imp_.SocketAccept();

	return ec;
}

ErrCode WSServer::StopAccept()
{
	ErrCode ec;

	auto& imp_ = *ImpUPtr_;

	imp_.Acceptor_.close(ec);

	return ec;
}

WSServer::Listener& WSServer::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}