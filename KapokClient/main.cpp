#include "TCPClient.h"
#include "TcpSession.h"
#include "AsynConcurrentPool.h"

#include <iostream>

using namespace std::string_literals;

int main()
{
	AsynConcurrentPool threadPool( 1 );

	TCPClient client(threadPool.GetIOService());

	client.SetServer( "127.0.0.1", 14 );

	threadPool.Start();

	TcpSessionSPtr session;
	ErrCode ec;
	std::tie(ec, session) = client.CreateSession();

	session->GetListener().OnPostSend_.connect( [&session]( auto se, const auto& ec )
	{
		if ( !ec )
		{
			session->GetListener().OnPostReceive_.connect( []( auto se, const auto& ec, TcpSession::BufferType& buf )
			{
				std::cout << buf.data() << std::endl;
			} );
		}
	} );

	auto testBuf = "12345"s;

	session->Send( boost::asio::buffer( testBuf ) );
	

	threadPool.Join();
}