#include "WSPPClient.h"
#include "WSPPSession.h"
#include "AsyncThreadPool.h"

#include <iostream>
#include <atomic>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>

using namespace std::string_literals;

WSPPSessionSPtr session;
std::atomic_uint32_t count{ 0 };

int main(int argc, char** argv)
{
	if ( argc == 1 )
	{
		return 0;
	}

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	AsyncThreadPool threadPool;
	threadPool.Start( 1 );

	if ( boost::filesystem::exists( "recvData" ) )
	{
		boost::filesystem::remove_all( "recvData" );
	}
	boost::filesystem::create_directories( "recvData" );

	WSPPClient client( threadPool.GetIOService() );
	client.GetListener().OnCreateSession_.connect( []( WSPPSessionSPtr se )
	{
		session = se;

		session->GetListener().OnMessage_.connect( []( auto se, MessagePtr msg )
		{
			auto curCount = count.fetch_add( 1 );

			boost::filesystem::ofstream ofs( "recvData/output" + std::to_string( curCount ), std::ios_base::binary |std::ios_base::trunc);
			auto& content = msg->get_payload();
			ofs.write( content.data(), content.size() );
		} );
	} );

	client.SetServer( argv[1] );

	auto ec = client.CreateSession();
	if ( ec )
	{
		auto i = 0;
	}

	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

	auto testBuf = R"({"MessageName":"RenderRequest"})"s;

	session->Send( boost::asio::buffer( testBuf ), websocketpp::frame::opcode::text );


	threadPool.Join();
}