#include "WSPPClient.h"
#include "WSPPSession.h"
#include "AsyncThreadPool.h"

#include <iostream>

using namespace std::string_literals;

WSPPSessionSPtr session;

int main(int argc, char** argv)
{
	if ( argc == 1 )
	{
		return 0;
	}

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	AsyncThreadPool threadPool;
	threadPool.Start( 1 );

	WSPPClient client( threadPool.GetIOService() );
	client.GetListener().OnCreateSession_.connect( []( WSPPSessionSPtr se )
	{
		session = se;
	} );

	client.SetServer( argv[1] );

	auto ec = client.CreateSession();
	if ( ec )
	{
		auto i = 0;
	}

	std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

	session->GetListener().OnPostSend_.connect( []( auto se )
	{
		session->GetListener().OnMessage_.connect( []( auto se, const auto& msgPtr )
		{
			std::cout << msgPtr->get_payload() << std::endl;
		} );
	} );

	auto testBuf = R"({"MessageName":"Echo","Content":"Hello"})"s;

	session->Send( boost::asio::buffer( testBuf ), websocketpp::frame::opcode::text );


	threadPool.Join();
}