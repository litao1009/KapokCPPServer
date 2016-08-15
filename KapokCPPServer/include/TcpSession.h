#pragma once

#include "TcpSessionFwd.h"

#include "Predef.h"

#include <boost/optional.hpp>
#include <boost/signals2.hpp>

class	TcpSession : public std::enable_shared_from_this<TcpSession>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	TcpSession(Socket& sock);

	~TcpSession();

	TcpSession(const TcpSession&) = delete;

public:

	using SessionBuffer = std::vector<uint8_t>;

	class	Listener
	{
	public:

		boost::signals2::signal<void(TcpSessionSPtr, const ErrCode&, uint32_t, uint32_t)>	OnReceive_;
		boost::signals2::signal<void(TcpSessionSPtr, const ErrCode&, SessionBuffer&)>		OnPostReceive_;
		boost::signals2::signal<void(TcpSessionSPtr, const ErrCode&, uint32_t, uint32_t)>	OnSend_;
		boost::signals2::signal<void(TcpSessionSPtr, const ErrCode&)>						OnPostSend_;
		boost::signals2::signal<void(TcpSessionSPtr)>										OnClose_;
	};

public:

	bool				Receive();

	void				ShutDown(bool clearOnRecv = false);

	bool				IsSending() const;
	
	bool				Send(const boost::asio::const_buffer& buffer);

	Socket&				GetSocket();

	const EndPoint&		GetRemoteEP() const;

	const EndPoint&		GetLocalEP() const;

	Listener&			GetListener();
};