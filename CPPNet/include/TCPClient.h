#pragma once

#include "TcpSessionFwd.h"
#include "Predef.h"

#include <memory>

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

class	TCPClient
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	TCPClient(IOService& ios);

	~TCPClient();

	TCPClient(const TCPClient&) = delete;

	TCPClient(TCPClient&& rhs);

public:

	class	Listener
	{
	public:

		boost::signals2::signal<void(ErrCode, TcpSessionSPtr)>	OnCreateSession_;
	};

public:

	std::tuple<ErrCode, TcpSessionSPtr>	CreateSession();

	void				AsyncCreateSession(boost::optional<uint32_t> timeout = boost::none);

	void				SetServer(const std::string& address, uint16_t port);

	Listener&			GetListener();
};