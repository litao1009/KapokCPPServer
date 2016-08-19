#pragma once

#include "WSPPPredef.h"
#include "WSPPSessionFwd.h"

#include <boost/signals2.hpp>

class	WSPPSession : public std::enable_shared_from_this<WSPPSession>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	WSPPSession(websocketpp::connection_hdl hdl);

	~WSPPSession();

	WSPPSession(const WSPPSession&) = delete;

	WSPPSession(WSPPSession&& rhs);

public:

	using	RecvBuf = std::vector<char>;

	class	Listener
	{
	public:

		boost::signals2::signal<void(WSPPSessionSPtr&, const ErrCode&)>	OnError_;
		boost::signals2::signal<void(WSPPSessionSPtr&)>					OnOpen_;
		boost::signals2::signal<void(WSPPSessionSPtr&)>					OnClose_;
		boost::signals2::signal<void(WSPPSessionSPtr&, const ErrCode&)>	OnPostSend_;
		boost::signals2::signal<void(WSPPSessionSPtr&, MessageType&)>	OnMessage_;
	};

public:

	ErrCode				Close();

	bool				Receive();
	
	bool				Send(const boost::asio::const_buffer& buffer, websocketpp::frame::opcode::value opCode);

	const EndPoint&		GetRemoteEP() const;

	const EndPoint&		GetLocalEP() const;

	Listener&			GetListener();
};