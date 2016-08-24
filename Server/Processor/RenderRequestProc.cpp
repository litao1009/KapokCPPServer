#pragma warning(disable:4503)

#include "RenderRequestProc.h"

#include "TestUnit/ServerTestUnit.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace std::string_literals;

IMPLEMNET_REFLECTION_WITH_KEY(RenderRequestProc, RenderRequest)

class	RenderRequestProc::Imp
{
public:

	class	SendInfo
	{
	public:

		ContextInfoSPtr	Context_;
		websocketpp::frame::opcode::value	OpCode_{ websocketpp::frame::opcode::text };
		std::string							Subfix_;
		uint32_t							ToSendFrame_{ 0 };
		boost::filesystem::path				DataPath_;
	};
	using	SendInfoSPtr = std::shared_ptr<SendInfo>;

	void	Test( ContextInfoSPtr& procInfo, JsonDOM& dom )
	{
		auto& context = *procInfo;
		auto sendPtr = std::make_shared<SendInfo>();
		auto& sendInfo = *sendPtr;
		sendInfo.Context_ = std::move( procInfo );

		if ( dom.HasMember( "ProtoType" ) && dom["ProtoType"].GetString() == "Protobuf" )
		{
			sendInfo.DataPath_ = "SampleData/sampleData_Protobuf";
			sendInfo.Subfix_ = "Msg";
			sendInfo.OpCode_ = websocketpp::frame::opcode::binary;
		}
		else
		{
			sendInfo.DataPath_ = "SampleData/sampleData_Json";
			sendInfo.OpCode_ = websocketpp::frame::opcode::text;
		}

		if ( !boost::filesystem::exists( sendInfo.DataPath_ ) )
		{
			return;
		}

		context.Session_->GetListener().OnPostSend_.connect( [sendInfo = std::move(sendPtr)](auto&) mutable
		{
			SendFrame( sendInfo );
		} );

		auto assetPath = sendInfo.DataPath_ / ("sampleData.asset" + sendInfo.Subfix_);
		std::string sendBuf( static_cast<uint32_t>( boost::filesystem::file_size( assetPath ) ), 0 );
		{
			boost::filesystem::ifstream ifs( assetPath, std::ios_base::binary );
			ifs.read( &sendBuf[0], sendBuf.size() );
		}

		context.Session_->Send( boost::asio::buffer( sendBuf ), sendInfo.OpCode_ );
	}

	static void	SendFrame( SendInfoSPtr& sendInfo )
	{
		auto& info_ = *sendInfo;
		auto& context_ = *(info_.Context_);

		context_.Session_->GetListener().OnPostSend_.disconnect_all_slots();

		if ( info_.ToSendFrame_ != 59 )
		{
			context_.Session_->GetListener().OnPostSend_.connect( [info = std::move(sendInfo)](auto&) mutable
			{
				info->ToSendFrame_ += 1;
				SendFrame( info );
			} );
		}

		auto assetPath = info_.DataPath_ / ( "sampleData.frameResponse" + std::to_string(info_.ToSendFrame_) + info_.Subfix_ );
		std::string sendBuf( static_cast<uint32_t>( boost::filesystem::file_size( assetPath ) ), 0 );
		{
			boost::filesystem::ifstream ifs( assetPath, std::ios_base::binary );
			ifs.read( &sendBuf[0], sendBuf.size() );
		}

		context_.Session_->Send( boost::asio::buffer( sendBuf ), info_.OpCode_ );
	}
};

RenderRequestProc::RenderRequestProc():ImpUPtr_(std::make_unique<Imp>())
{

}

RenderRequestProc::~RenderRequestProc()
{

}

void RenderRequestProc::ProcessJSON( ContextInfoSPtr& procInfo, JsonDOM& dom )
{
	auto& imp_ = *ImpUPtr_;

	imp_.Test( procInfo, dom );
}
