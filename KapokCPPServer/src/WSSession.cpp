#include "WSSession.h"

#include "WSPredef.h"

class	WSSession::Imp
{
public:

	Imp(IOService& ios):WSStream_(ios)
	{}

	beast::streambuf	RecvBuf_;
	WSStream			WSStream_;

	EndPoint			RemoteEP_;
	EndPoint			LocalEP_;

	Listener			Listener_;
};

WSSession::WSSession(IOService& ios):ImpUPtr_(std::make_unique<Imp>(ios))
{

}

WSSession::~WSSession()
{

}

bool WSSession::Receive()
{
	auto& imp_ = *ImpUPtr_;

	return true;
}

ErrCode WSSession::Close()
{
	auto& imp_ = *ImpUPtr_;

	ErrCode ec;
	imp_.WSStream_.close(beast::websocket::close_code::normal, ec);

	return ec;
}

bool WSSession::Send(const boost::asio::const_buffer& buffer)
{
	auto& imp_ = *ImpUPtr_;

	return true;
}

WSStream& WSSession::GetStream()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.WSStream_;
}

const EndPoint& WSSession::GetRemoteEP() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.RemoteEP_;
}

const EndPoint& WSSession::GetLocalEP() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.LocalEP_;
}

WSSession::Listener& WSSession::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}