#pragma once

#include "WSPPPredef.h"
#include "WSPPSessionFwd.h"

#include <memory>

#include <boost/signals2.hpp>

class	WSPPServer
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	WSPPServer(IOService& ios);

	~WSPPServer();

	WSPPServer(const WSPPServer&) = delete;

	WSPPServer(WSPPServer&& rhs);

public:

	class	Listener
	{
	public:

		boost::signals2::signal<void(WSPPSessionSPtr&)>	OnAccept_;
		boost::signals2::signal<void(const ErrCode&)>	OnAcceptError_;
	};

public:

	ErrCode		StartAccept(uint16_t listenPort);

	ErrCode		StopAccept();

	Listener&	GetListener();
};
