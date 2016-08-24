#include "TCPClient.h"
#include "TcpSession.h"

#include "ErrCodeUtil.h"

class	TCPClient::Imp
{
public:

	Imp(IOService& ios):IOService_(ios)
	{}

	IOService&	IOService_;
	boost::asio::ip::tcp::endpoint	ServerEP_;
	Listener						Listener_;
};

TCPClient::TCPClient(IOService& ios):ImpUPtr_(std::make_unique<Imp>(ios))
{

}

TCPClient::~TCPClient()
{

}

TCPClient::TCPClient(TCPClient && rhs):ImpUPtr_(std::move(rhs.ImpUPtr_))
{
}

std::tuple<ErrCode, TcpSessionSPtr> TCPClient::CreateSession()
{
	auto& imp_ = *ImpUPtr_;

	auto session = std::make_shared<TcpSession>(imp_.IOService_);

	boost::system::error_code ec;
	session->GetSocket().connect(imp_.ServerEP_, ec);

	return std::make_tuple( ConvertBoostECToStdEC( ec ), session);
}

void TCPClient::AsyncCreateSession(boost::optional<uint32_t> timeout /*= boost::none*/)
{
	auto& imp_ = *ImpUPtr_;

	auto session = std::make_shared<TcpSession>(imp_.IOService_);

	session->GetSocket().async_connect(imp_.ServerEP_, [this, session](const auto& ec)
	{
		auto& imp_ = *ImpUPtr_;

		imp_.Listener_.OnCreateSession_( ConvertBoostECToStdEC( ec ), session);
	});
}

void TCPClient::SetServer(const std::string& address, uint16_t port)
{
	auto& imp_ = *ImpUPtr_;

	imp_.ServerEP_ = decltype( imp_.ServerEP_ )( boost::asio::ip::address::from_string(address), port );
}

TCPClient::Listener& TCPClient::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}