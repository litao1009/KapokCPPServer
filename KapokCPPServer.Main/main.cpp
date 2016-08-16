#include "TCPServer.h"
#include "TcpSession.h"
#include "AsyncThreadPool.h"

#include <vector>
#include <iostream>

int main()
{
	std::vector<TcpSessionSPtr> sessionList;

	AsyncThreadPool threadPool(1);
	threadPool.Start();

	TCPServer tcpserver(threadPool.GetIOService());

	tcpserver.GetListener().OnAccept_.connect([&sessionList](TcpSessionSPtr& session)
	{
		session->GetListener().OnPostReceive_.connect([](TcpSessionSPtr& thisPtr, const ErrCode& ec, TcpSession::BufferType& buf)
		{
			if ( !ec )
			{
				std::cout << buf.data() << std::endl;
				thisPtr->Send(boost::asio::buffer(buf));
			}
		});

		session->GetListener().OnPostSend_.connect([](TcpSessionSPtr& thisPtr, const ErrCode& ec)
		{
			if ( !ec )
			{
				thisPtr->Receive();
			}
		});
		
		session->Receive();

		sessionList.push_back(session);
	});

	tcpserver.StartAccept( 13 );

	threadPool.Join();
}