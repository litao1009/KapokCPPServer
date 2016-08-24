#include "WSPPClient.h"
#include "WSPPSession.h"

#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_no_tls_client.hpp"

class	WSPPClient::Imp
{
public:

	using	ClientType = websocketpp::client<websocketpp::config::asio_client>;

public:

	Imp( IOService& ios ) :IOS_( ios )
	{}

	ClientType	Client_;
	Listener	Listener_;
	std::string	Server_;
	IOService&	IOS_;
	WSPPClient*	ThisPtr_{};
};

WSPPClient::WSPPClient( IOService& ios ):ImpUPtr_(std::make_unique<Imp>(ios))
{
	auto& imp_ = *ImpUPtr_;
	imp_.ThisPtr_ = this;

	imp_.Client_.set_access_channels( websocketpp::log::alevel::none );
	imp_.Client_.init_asio( &imp_.IOS_ );

	imp_.Client_.set_open_handler( [thisPtr = imp_.ThisPtr_]( websocketpp::connection_hdl connHDL )
	{
		auto& imp_ = *( thisPtr->ImpUPtr_ );
		ErrCode ec;
		auto conn = imp_.Client_.get_con_from_hdl( connHDL );

		auto session = std::make_shared<WSPPSession>( connHDL );
		WSPPSessionWPtr sessionWPtr = session;

		conn->set_open_handler( [sessionWPtr]( websocketpp::connection_hdl connHDL )
		{
			auto sptr = sessionWPtr.lock();
			sptr->GetListener().OnOpen_( sptr );
		} );

		conn->set_close_handler( [sessionWPtr]( websocketpp::connection_hdl connHDL )
		{
			auto sptr = sessionWPtr.lock();
			sptr->GetListener().OnClose_( sptr );
		} );

		conn->set_message_handler( [sessionWPtr]( websocketpp::connection_hdl hdl, Imp::ClientType::message_ptr msg )
		{
			auto sptr = sessionWPtr.lock();
			sptr->GetListener().OnMessage_( sptr, msg );
		} );

		conn->set_send_handler( [sessionWPtr]( websocketpp::connection_hdl connHDL )
		{
			auto sptr = sessionWPtr.lock();
			sptr->GetListener().OnPostSend_( sptr );
		} );

		imp_.Listener_.OnCreateSession_( session );
	} );
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

ErrCode WSPPClient::CreateSession()
{
	auto& imp_ = *ImpUPtr_;

	ErrCode ec;

	auto conn = imp_.Client_.get_connection( imp_.Server_, ec );
	if ( ec )
	{
		return ec;
	}

	imp_.Client_.connect( conn );
	
	return ec;
}

WSPPClient::Listener & WSPPClient::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}
