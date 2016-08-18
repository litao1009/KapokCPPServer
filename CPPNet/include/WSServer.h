#pragma once

#include "WSPredef.h"
#include "WSSessionFwd.h"

#include <memory>

#include <boost/signals2.hpp>

class	WSServer
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	WSServer(IOService& ios);

	~WSServer();

	WSServer(const WSServer&) = delete;

	WSServer(WSServer&& rhs);

public:

	class	Listener
	{
	public:

		boost::signals2::signal<void(WSSessionSPtr&)>	OnAccept_;
		boost::signals2::signal<void(const ErrCode&)>	OnAcceptError_;
	};

public:

	ErrCode		StartAccept(uint16_t listenPort);

	ErrCode		StopAccept();

	Listener&	GetListener();
};
