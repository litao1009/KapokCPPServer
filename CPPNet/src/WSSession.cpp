#pragma warning(disable:4503)

#include "WSSession.h"

#include "WSPredef.h"

#include "ErrCodeUtil.h"

class	WSSession::Imp
{
public:

	using	RecvBuf = std::vector<char>;

public:

	Imp(IOService& ios):WSStream_(ios)
	{}

	beast::websocket::frame_info	FrameInfo_;
	beast::streambuf				RecvBuf_;
	WSStream						WSStream_;

	EndPoint						RemoteEP_;
	EndPoint						LocalEP_;

	Listener						Listener_;

public:

	void	Receive(WSSessionSPtr& session)
	{
		
		WSStream_.async_read_frame(FrameInfo_, RecvBuf_, [this, sessionPtr = std::move(session)](const auto& ec) mutable
		{
			if (ec)
			{
				if (ec == beast::websocket::error::closed)
				{
					Listener_.OnClose_(ErrCode(), sessionPtr);
				}

				return;
			}
		
			if (0 == FrameInfo_.fin)
			{
				Receive(sessionPtr);
			}
			else
			{
				Listener_.OnMessage_(FrameInfo_.op, sessionPtr);
			}
		});
	}

	void	Send(WSSessionSPtr& session)
	{
		WSStream_.async_write(RecvBuf_.data(), [this, sessionPtr = std::move(session)](const auto& ec) mutable
		{
			Listener_.OnPostSend_(ConvertBoostECToStdEC(ec), sessionPtr);
		});
	}
};

WSSession::WSSession(IOService& ios):ImpUPtr_(std::make_unique<Imp>(ios))
{
	auto& imp_ = *ImpUPtr_;
}

WSSession::~WSSession()
{

}

WSSession::WSSession(WSSession && rhs) :ImpUPtr_(std::move(rhs.ImpUPtr_))
{
}

bool WSSession::Receive()
{
	auto& imp_ = *ImpUPtr_;

	imp_.Receive(shared_from_this());

	return true;
}

ErrCode WSSession::Close()
{
	auto& imp_ = *ImpUPtr_;

	boost::system::error_code ec;
	imp_.WSStream_.close(beast::websocket::close_code::normal, ec);

	imp_.WSStream_.next_layer().close(ec);

	auto stdEc = ConvertBoostECToStdEC( ec );

	imp_.Listener_.OnClose_( stdEc, shared_from_this());

	return stdEc;
}

bool WSSession::Send(const boost::asio::const_buffer& buffer, beast::websocket::opcode opCode)
{
	auto& imp_ = *ImpUPtr_;
	auto bufSize = boost::asio::buffer_size(buffer);

	imp_.RecvBuf_.consume(imp_.RecvBuf_.size());
	imp_.RecvBuf_.commit(boost::asio::buffer_copy(imp_.RecvBuf_.prepare(bufSize), buffer));

	imp_.WSStream_.set_option(beast::websocket::message_type(opCode));

	imp_.Send(shared_from_this());

	return true;
}

WSStream& WSSession::GetStream()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.WSStream_;
}

beast::streambuf& WSSession::GetRecvBuf()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.RecvBuf_;
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