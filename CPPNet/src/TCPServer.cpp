#include "TCPServer.h"
#include "TcpSession.h"

#include "ErrCodeUtil.h"

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
		auto session = std::make_shared<TcpSession>(Acceptor_.get_io_service());
		auto& sock = session->GetSocket();
		Acceptor_.async_accept(sock, [this, sessionPtr = std::move(session)](const auto& ec) mutable
		{
			if ( ec )
			{
				Listener_.OnAcceptError_( ConvertBoostECToStdEC( ec ) );
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

TCPServer::TCPServer(TCPServer && rhs) :ImpUPtr_(std::move(rhs.ImpUPtr_))
{
}

ErrCode TCPServer::StartAccept(uint16_t listenPort)
{
	boost::system::error_code ec;

	auto& imp_ = *ImpUPtr_;	

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), listenPort);
	
	imp_.Acceptor_.open(ep.protocol(), ec);
	if ( ec )
	{
		return ConvertBoostECToStdEC( ec );
	}

	imp_.Acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
	if ( ec )
	{
		return ConvertBoostECToStdEC( ec );
	}

	imp_.Acceptor_.bind(ep, ec);
	if ( ec )
	{
		return ConvertBoostECToStdEC( ec );
	}

	imp_.Acceptor_.listen(boost::asio::socket_base::max_connections, ec);
	if ( ec )
	{
		return ConvertBoostECToStdEC( ec );
	}

	imp_.Accept();

	return ConvertBoostECToStdEC( ec );
}

ErrCode TCPServer::StopAccept()
{
	boost::system::error_code ec;

	auto& imp_ = *ImpUPtr_;

	imp_.Acceptor_.close(ec);

	return ConvertBoostECToStdEC( ec );
}

TCPServer::Listener& TCPServer::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}