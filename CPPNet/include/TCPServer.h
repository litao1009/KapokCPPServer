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

		boost::signals2::signal<void(TcpSessionSPtr&)>	OnAccept_;
		boost::signals2::signal<void(const ErrCode&)>	OnAcceptError_;
	};

public:

	ErrCode		StartAccept(uint16_t listenPort);

	ErrCode		StopAccept();

	Listener&	GetListener();
};