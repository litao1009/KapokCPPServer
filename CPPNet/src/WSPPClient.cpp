#include "WSPPClient.h"

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"

class	WSPPClient::Imp
{
public:

	using	ClientType = websocketpp::client<websocketpp::config::asio_client>;

public:

	ClientType	Client_;
	Listener	Listener_;
	std::string	Server_;
};

WSPPClient::WSPPClient():ImpUPtr_(std::make_unique<Imp>())
{

}

WSPPClient::~WSPPClient()
{

}

WSPPClient::WSPPClient( WSPPClient && rhs ):ImpUPtr_(std::move(rhs.ImpUPtr_))
{}

void WSPPClient::SetServer( const std::string & server )
{
	auto& imp_ = *ImpUPtr_;

	imp_.Server_ = server;
}

std::tuple<ErrCode, WSPPSessionSPtr> WSPPClient::CreateSession()
{
	auto& imp_ = *ImpUPtr_;
	ErrCode ret;
	std::error_code ec;
	auto conn = imp_.Client_.get_connection( imp_.Server_, ec );
	
	return std::tuple<ErrCode, WSPPSessionSPtr>();
}

WSPPClient::Listener & WSPPClient::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}
