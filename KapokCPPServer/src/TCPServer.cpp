#include "TCPServer.h"
#include "TcpSession.h"

#include <boost/asio.hpp>

class	TCPServer::Imp
{
public:

	Imp(IOService& ios):Acceptor_(ios)
	{}

	boost::asio::ip::tcp::acceptor	Acceptor_;
	Listener						Listener_;

public:

	void	Accept()
	{
		Socket sock(Acceptor_.get_io_service());
		auto session = std::make_shared<TcpSession>(sock);

		Acceptor_.async_accept(session->GetSocket(), [this, sessionPtr = std::move(session)](const auto& ec) mutable
		{
			if ( ec )
			{
				Listener_.OnAcceptError_(ec);
			}
			else
			{
				Listener_.OnAccept_(sessionPtr);

				if ( Acceptor_.is_open() )
				{
					Accept();
				}
			}
		});
	}
};

TCPServer::TCPServer(IOService& ios):ImpUPtr_(std::make_unique<Imp>(ios))
{
	auto& imp_ = *ImpUPtr_;
}

TCPServer::~TCPServer()
{

}

boost::system::error_code TCPServer::StartAccept(uint16_t listenPort)
{
	boost::system::error_code ec;

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

	return ec;
}

boost::system::error_code TCPServer::StopAccept()
{
	boost::system::error_code ec;

	auto& imp_ = *ImpUPtr_;

	imp_.Acceptor_.close(ec);

	return ec;
}

TCPServer::Listener& TCPServer::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}