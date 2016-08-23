#pragma warning(disable:4503)

#include "EchoProc.h"

IMPLEMNET_REFLECTION_WITH_KEY(EchoProc, Echo)

void EchoProc::Process(SProcInfoSPtr& procInfo)
{
	ContentType response;
	response.SetObject();
	response.AddMember( "MessageName", "EchoResponse", response.GetAllocator() );
	if ( procInfo->Content_.HasMember( "Content" ) )
	{
		auto& content = procInfo->Content_["Content"];
		response.AddMember( "Content", content, response.GetAllocator() );
	}
	else
	{
		response.AddMember( "Content", R"(Need 'Content' field.)", response.GetAllocator() );
	}

	auto sendBuf = WriteJson(response);

	procInfo->Session_->GetListener().OnPostSend_.connect([](auto& session)
	{
		session->Receive();
	});

	procInfo->Session_->Send(boost::asio::buffer(sendBuf), procInfo->RawMsg_->get_opcode());
}
