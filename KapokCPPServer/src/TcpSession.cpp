#include "TcpSession.h"

#include <atomic>
#include <array>

class	TcpSession::Imp
{
public:

	enum ERecvState
	{
		ERS_None,
		ERS_Head,
		ERS_Body,
		ERS_Count
	};

	enum ESendState
	{
		ESS_None,
		ESS_Head,
		ESS_Body,
		ESS_Count
	};

	class	SHead
	{
	public:

		using	StartTag = std::integral_constant<uint32_t, 0xffffffff>;
		using	EndTag = std::integral_constant<uint32_t, 0xfefefefe>;

	public:

		uint32_t	StartTag_{ StartTag::value };
		uint32_t	HeadCRC_{ 0 };
		uint32_t	ContentLength_{ 0 };
		uint32_t	ContentCRC_{ 0 };
		uint32_t	EndTag_{ EndTag::value };

	public:

		bool	Check()
		{
			if ( StartTag_ != StartTag::value )
			{
				return false;
			}

			if ( EndTag_ != EndTag::value )
			{
				return false;
			}

			return true;
		}
	};

	using	HeadSize = std::integral_constant<uint32_t, sizeof(SHead)>;

	using	HeadBuf = std::array<uint8_t, 0xff>;
	

	std::atomic<ERecvState>		RecvState_{ ERS_None };
	std::atomic<ESendState>		SendState_{ ESS_None };
	Socket						Socket_;
	HeadBuf						HeadBuf_;
	BufferType					BodyBuf_;
	uint32_t					RecvOffset_{};
	uint32_t					RecvTransfered_{};
	SHead						RecvHead_;
	boost::asio::const_buffer	SendBuf_;
	uint32_t					SendTransfered_{};
	SHead						SendHead_;
	Listener					Listener_;
	EndPoint					LocalEP_;
	EndPoint					RemoteEP_;
	TcpSession*					ThisPtr_{};

public:

	Imp(Socket& sock):Socket_(std::move(sock))
	{}

public:

	void	Close()
	{
		ErrCode	ec;
		Socket_.shutdown(Socket::shutdown_both, ec);

		Socket_.close(ec);

		Listener_.OnClose_(ThisPtr_->shared_from_this());
	}

	void	ResetRecv()
	{
		RecvState_ = ERS_None;
		RecvOffset_ = 0;
		RecvTransfered_ = 0;
	}

	void	ResetSend()
	{
		SendState_ = ESS_None;
		SendTransfered_ = 0;
	}

	void	RecvHead(TcpSessionSPtr& sessionPtr)
	{
		if ( RecvState_ == ERS_None )
		{
			RecvOffset_ = 0;
			RecvTransfered_ = 0;

			RecvState_ = ERS_Head;
		}

		Socket_.async_receive(boost::asio::buffer(HeadBuf_.data() + RecvTransfered_, HeadBuf_.size() - RecvTransfered_), [this, thisPtr = std::move(sessionPtr)](const auto& ec, auto transBytes) mutable
		{
			if ( ec )
			{
				ResetRecv();
				Listener_.OnPostReceive_(thisPtr, ec, BufferType(0));
				return;
			}

			RecvTransfered_ += transBytes;

			auto readyToReadLength = RecvTransfered_ - RecvOffset_;

			if ( readyToReadLength >= HeadSize::value )
			{
				auto pReadBegin = HeadBuf_.data() + RecvOffset_;
				auto pReadEnd = pReadBegin + readyToReadLength - HeadSize::value + 1;
				for ( ; pReadBegin < pReadEnd; ++pReadBegin )
				{
					auto& head = *reinterpret_cast<SHead*>( pReadBegin );
					if ( head.Check() )
					{//Begin read body
						RecvHead_ = head;
						
						auto pBufEnd = HeadBuf_.data() + RecvTransfered_;

						BodyBuf_.resize(RecvHead_.ContentLength_, 0);
						BodyBuf_.shrink_to_fit();

						auto pCopyBegin = pReadBegin + HeadSize::value;
						auto pCopyEnd = HeadBuf_.data() + RecvTransfered_ + 1;
						auto pDst = std::copy(pCopyBegin, pCopyEnd, BodyBuf_.data());

						RecvOffset_ = 0;
						RecvTransfered_ = pDst - BodyBuf_.data();
						RecvState_ = ERS_Body;
						RecvBody(thisPtr);

						return;
					}
					else
					{//continue
						++RecvOffset_;
					}
				}
			}

			if ( HeadBuf_.size() - RecvTransfered_ < HeadSize::value )
			{
				auto pDst = std::copy(HeadBuf_.data() + RecvOffset_, HeadBuf_.data() + RecvTransfered_, BodyBuf_.data());
				pDst = std::copy(BodyBuf_.data(), pDst, HeadBuf_.data());

				RecvOffset_ = 0;
				RecvTransfered_ = pDst - HeadBuf_.data();
			}

			RecvHead(thisPtr);
		});
	}

	void	RecvBody(TcpSessionSPtr& sessionPtr)
	{
		Socket_.async_receive(boost::asio::buffer(BodyBuf_.data() + RecvTransfered_, BodyBuf_.size() - RecvTransfered_), [this, thisPtr = std::move(sessionPtr)](const auto& ec, auto transBytes) mutable
		{
			if ( ec )
			{
				ResetRecv();
				Listener_.OnPostReceive_(thisPtr, ec, BufferType(0));
				return;
			}

			RecvTransfered_ += transBytes;

			Listener_.OnReceive_(thisPtr, ec, RecvTransfered_, BodyBuf_.size());

			if ( RecvTransfered_ < BodyBuf_.size() )
			{
				RecvBody(thisPtr);
			}
			else
			{
				ResetRecv();
				Listener_.OnPostReceive_(thisPtr, ec, BodyBuf_);
			}
		});
	}

	void	SendHead(TcpSessionSPtr& sessionPtr)
	{
		if ( SendState_ == ESS_None )
		{
			SendState_ = ESS_Head;
			SendTransfered_ = 0;
		}
		auto& headBuf = *reinterpret_cast<HeadBuf*>( &SendHead_ );

		Socket_.async_send(boost::asio::buffer(headBuf.data() + SendTransfered_, headBuf.size() - SendTransfered_), [this, thisPtr = std::move(sessionPtr)](const auto& ec, auto ubytes) mutable
		{
			if ( ec )
			{
				Listener_.OnPostSend_(thisPtr, ec);
				ResetSend();
				return;
			}

			assert(ubytes == HeadSize::value);

			SendTransfered_ = 0;
			SendBody(thisPtr);
		});
	}

	void	SendBody(TcpSessionSPtr& sessionPtr)
	{
		auto bufData = boost::asio::buffer_cast<const uint8_t*>(SendBuf_);
		auto bufSize = boost::asio::buffer_size(SendBuf_);

		Socket_.async_write_some(boost::asio::buffer(bufData + SendTransfered_, bufSize - SendTransfered_), [this, thisPtr = std::move(sessionPtr)](const auto& ec, auto ubytes) mutable
		{
			if ( ec )
			{
				Listener_.OnPostSend_(thisPtr, ec);
				ResetSend();
				return;
			}

			SendTransfered_ += ubytes;

			Listener_.OnSend_(thisPtr, ec, SendTransfered_, boost::asio::buffer_size(SendBuf_));

			if ( SendTransfered_ == boost::asio::detail::buffer_size_helper(SendBuf_) )
			{
				ResetSend();
				Listener_.OnPostSend_(thisPtr, ec);
			}
			else
			{
				SendBody(thisPtr);
			}
		});
	}
};

TcpSession::TcpSession(Socket& sock):ImpUPtr_(std::make_unique<Imp>(sock))
{
	auto& imp_ = *ImpUPtr_;

	imp_.ThisPtr_ = this;
	imp_.LocalEP_ = imp_.Socket_.local_endpoint();
	imp_.RemoteEP_ = imp_.Socket_.remote_endpoint();
}

TcpSession::~TcpSession()
{

}

bool TcpSession::Receive()
{
	auto& imp_ = *ImpUPtr_;

	if ( imp_.RecvState_ != Imp::ERS_None )
	{
		return false;
	}

	if ( !imp_.Socket_.is_open() )
	{
		return false;
	}

	imp_.RecvHead(shared_from_this());

	return true;
}

void TcpSession::ShutDown(bool clearOnRecv /*= false*/)
{
	auto& imp_ = *ImpUPtr_;

	imp_.Close();
}

bool TcpSession::IsSending() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.SendState_ != imp_.ERS_None;
}

bool TcpSession::Send(const boost::asio::const_buffer& buffer)
{
	auto& imp_ = *ImpUPtr_;

	if ( imp_.SendState_ != imp_.ESS_None )
	{
		return false;
	}

	imp_.SendHead_.ContentLength_ = boost::asio::detail::buffer_size_helper(buffer);
	imp_.SendBuf_ = buffer;

	imp_.SendHead(shared_from_this());

	return true;
}

Socket& TcpSession::GetSocket()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Socket_;
}

const EndPoint& TcpSession::GetRemoteEP() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.RemoteEP_;
}

const EndPoint& TcpSession::GetLocalEP() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.LocalEP_;
}

TcpSession::Listener& TcpSession::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}