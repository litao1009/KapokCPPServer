#pragma once

#include "Predef.h"
#include "TcpSessionFwd.h"

#include <memory>

#include <boost/signals2.hpp>

class	TCPServer
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	TCPServer(IOService& ios);

	~TCPServer();

	TCPServer(const TCPServer&) = delete;

public:

	class	Listener
	{
	public:

		boost::signals2::signal<void(TcpSessionSPtr&)>					OnAccept_;
		boost::signals2::signal<void(const boost::system::error_code&)>	OnAcceptError_;
	};

public:

	boost::system::error_code	StartAccept(uint16_t listenPort);

	boost::system::error_code	StopAccept();

	Listener&	GetListener();
};