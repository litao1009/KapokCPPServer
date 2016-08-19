#pragma warning(disable:4503)

#include "EchoProc.h"

IMPLEMNET_REFLECTION_WITH_KEY(EchoProc, Echo)

void EchoProc::Process(SProcInfoSPtr& procInfo)
{
	auto content = procInfo->Content_.get<std::string>("Content");
	Ptree response;
	response.add("MessageName", "EchoResponse");
	response.add("Content", content);

	auto sendBuf = WriteJson(response);

	procInfo->Session_->GetListener().OnPostSend_.connect([](auto& session)
	{
		session->Receive();
	});

	procInfo->Session_->Send(boost::asio::buffer(sendBuf), procInfo->RawMsg_->get_opcode());
}
