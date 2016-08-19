#include "WSPPSession.h"
class	WSPPSession::Imp
{
public:

	websocketpp::connection_hdl	ConnHDL_;

	EndPoint					RemoteEP_;
	EndPoint					LocalEP_;

	Listener					Listener_;
};

WSPPSession::WSPPSession(websocketpp::connection_hdl hdl):ImpUPtr_(std::make_unique<Imp>())
{
	auto& imp_ = *ImpUPtr_;

	imp_.ConnHDL_ = hdl;
}

WSPPSession::WSPPSession(WSPPSession&& rhs):ImpUPtr_(std::move(rhs.ImpUPtr_))
{

}

WSPPSession::~WSPPSession()
{

}

ErrCode WSPPSession::Close()
{
	auto& imp_ = *ImpUPtr_;

	return{};
}

bool WSPPSession::Receive()
{
	auto& imp_ = *ImpUPtr_;

	return true;
}

bool WSPPSession::Send(const boost::asio::const_buffer& buffer, websocketpp::frame::opcode::value opCode)
{
	auto& imp_ = *ImpUPtr_;
	
	auto conn = std::static_pointer_cast<ConnectionType>(imp_.ConnHDL_.lock());

	auto bufferData = boost::asio::buffer_cast<const void*>(buffer);
	auto bufferSize = boost::asio::buffer_size(buffer);

	conn->send(bufferData, bufferSize, opCode);

	return true;
}

const EndPoint& WSPPSession::GetRemoteEP() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.RemoteEP_;
}

const EndPoint& WSPPSession::GetLocalEP() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.LocalEP_;
}

WSPPSession::Listener& WSPPSession::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}