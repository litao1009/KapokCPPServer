#pragma once

#include "WSSessionFwd.h"

#include "WSPredef.h"

#include <boost/signals2.hpp>

class	WSSession : public std::enable_shared_from_this<WSSession>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	WSSession(IOService& ios);

	~WSSession();

	WSSession(const WSSession&) = delete;

public:

	using	RecvBuf = std::vector<char>;

	class	Listener
	{
	public:

		boost::signals2::signal<void(const ErrCode&, WSSessionSPtr&)>	OnOpen_;
		boost::signals2::signal<void(const ErrCode&, WSSessionSPtr&)>	OnClose_;
		boost::signals2::signal<void(const ErrCode&, WSSessionSPtr&)>	OnPostSend_;
		boost::signals2::signal<void(const beast::websocket::opcode&, WSSessionSPtr&)>	OnMessage_;
	};

public:

	ErrCode				Close();

	bool				Receive();
	
	bool				Send(const boost::asio::const_buffer& buffer, beast::websocket::opcode opCode);

	WSStream&			GetStream();

	beast::streambuf&	GetRecvBuf();

	const EndPoint&		GetRemoteEP() const;

	const EndPoint&		GetLocalEP() const;

	Listener&			GetListener();
};