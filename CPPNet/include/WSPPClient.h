#pragma once

#include "Predef.h"
#include "WSPPSessionFwd.h"

#include <memory>

#include <boost/signals2.hpp>

class	WSPPClient
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	WSPPClient( IOService& ios );

	~WSPPClient();

	WSPPClient( const WSPPClient& ) = delete;

	WSPPClient( WSPPClient&& rhs );

public:

	class	Listener
	{
	public:

		boost::signals2::signal<void( WSPPSessionSPtr)>	OnCreateSession_;
	};

public:

	void	SetServer( const std::string& server );

	ErrCode	CreateSession();

	Listener&	GetListener();
};