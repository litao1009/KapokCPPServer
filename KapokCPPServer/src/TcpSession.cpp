#include "TcpSession.h"

#include <atomic>
#include <array>

class	TcpSession::Imp
{
public:

	enum ESessionState
	{
		ESS_Init,
		ESS_Working,
		ESS_Close,
		ESS_Count
	};

	enum ERecvState
	{
		ERS_None,
		ERS_Head,
		ERS_Body,
		ERS_Count
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
	using	RecvBuf = std::vector<uint8_t>;
	


	std::atomic<ESessionState>	State_{ ESS_Init };
	ERecvState					RecvState_{ ERS_None };
	Socket						Socket_;
	HeadBuf						HeadBuf_;
	RecvBuf						BodyBuf_;
	uint32_t					RecvReadOffset_{};
	uint32_t					RecvTransfered_{};
	SHead						RecvHead_;
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
		if ( State_ == ESS_Close )
		{
			return;
		}

		State_ = ESS_Close;

		ErrCode	ec;
		Socket_.shutdown(Socket::shutdown_both, ec);

		Socket_.close(ec);

		Listener_.OnClose_(ThisPtr_->shared_from_this());
	}

	void	Reset()
	{
		RecvState_ = ERS_None;
		RecvReadOffset_ = 0;
		RecvTransfered_ = 0;
	}

	bool	Start()
	{
		if ( !Socket_.is_open() )
		{
			return false;
		}

		if ( State_ != ESS_Init )
		{
			return false;
		}

		State_ = ESS_Working;

		RecvReadOffset_ = 0;
		RecvTransfered_ = 0;

		RecvState_ = ERS_Head;
		RecvHead(ThisPtr_->shared_from_this());

	}

	void	RecvHead(TcpSessionSPtr& sessionPtr)
	{
		Socket_.async_receive(boost::asio::buffer(HeadBuf_.data() + RecvTransfered_, HeadBuf_.size() - RecvTransfered_), [this, thisPtr = std::move(sessionPtr)](const ErrCode& ec, uint32_t transBytes) mutable
		{
			if ( ec )
			{
				Reset();
				Listener_.OnReceive_(thisPtr, ec, 0, 0);
				return;
			}

			RecvTransfered_ += transBytes;

			auto readyToReadLength = RecvTransfered_ - RecvReadOffset_;

			if ( readyToReadLength >= HeadSize::value )
			{
				auto pReadBegin = HeadBuf_.data() + RecvReadOffset_;
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

						RecvReadOffset_ = 0;
						RecvTransfered_ = pDst - BodyBuf_.data();

						RecvBody(thisPtr);
						return;
					}
					else
					{//continue
						++RecvReadOffset_;
					}
				}
			}

			if ( HeadBuf_.size() - RecvTransfered_ < HeadSize::value )
			{
				auto pDst = std::copy(HeadBuf_.data() + RecvReadOffset_, HeadBuf_.data() + RecvTransfered_, BodyBuf_.data());
				pDst = std::copy(BodyBuf_.data(), pDst, HeadBuf_.data());

				RecvReadOffset_ = 0;
				RecvTransfered_ = pDst - HeadBuf_.data();
			}

			RecvHead(thisPtr);
		});
	}

	void	RecvBody(TcpSessionSPtr& sessionPtr)
	{

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

bool TcpSession::StartRead()
{
	return true;
}

void TcpSession::ShutDown(bool clearOnRecv /*= false*/)
{

}

bool TcpSession::IsWorking()
{
	return true;
}

bool TcpSession::IsSending() const
{
	return true;
}

void TcpSession::Send(const boost::asio::const_buffer& buffer)
{

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