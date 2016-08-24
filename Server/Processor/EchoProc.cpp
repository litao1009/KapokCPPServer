#pragma warning(disable:4503)

#include "EchoProc.h"

IMPLEMNET_REFLECTION_WITH_KEY(EchoProc, Echo)

void EchoProc::ProcessJSON( ContextInfoSPtr& procInfo, JsonDOM& dom )
{
	JsonDOM response;
	response.SetObject();
	response.AddMember( "MessageName", "EchoResponse", response.GetAllocator() );
	if ( dom.HasMember( "Content" ) )
	{
		auto& content = dom["Content"];
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

	procInfo->Session_->Send(boost::asio::buffer(sendBuf), websocketpp::frame::opcode::text);
}
