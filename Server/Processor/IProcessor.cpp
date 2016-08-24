#include "IProcessor.h"

#include "WSPPSession.h"

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include "message/MessageName.pb.h"

#include <sstream>

void IProcessor::DispatchMsg(AsyncThreadPool& threadPool, MessagePtr& rawMsg, WSPPSessionSPtr& session)
{
	auto context = std::make_shared<ContextInfo>( threadPool );
	context->Session_ = std::move(session);

	switch ( rawMsg->get_opcode() )
	{
	case websocketpp::frame::opcode::text:
	{//JSON
		std::istringstream is( rawMsg->get_payload() );
		rapidjson::IStreamWrapper isw( is );

		JsonDOM dom;
		dom.SetObject();
		dom.ParseStream( isw );

		if ( dom.HasParseError() || !dom.HasMember( "MessageName" ) )
		{
			return;
		}

		auto proc = IProcessor::CreateByKey( dom["MessageName"].GetString() );
		if ( proc )
		{
			proc->ProcessJSON( context, dom );
		}
	}break;
	case websocketpp::frame::opcode::binary:
	{//Protobuf
		msg::MessageName protoMsg;
		auto ret = protoMsg.ParseFromString( rawMsg->get_payload() );

		if ( !ret )
		{
			return;
		}

		auto proc = IProcessor::CreateByKey( protoMsg.message_name() );
		if ( proc )
		{
			proc->ProcessProtobuf( context, rawMsg->get_raw_payload() );
		}
	}break;
	default:
		break;
	}
}

std::string IProcessor::WriteJson(const JsonDOM& input)
{
	std::stringstream ss;
	
	WriteJson( ss, input );

	return ss.str();
}

void IProcessor::WriteJson( std::ostream & os, const JsonDOM & input )
{
	rapidjson::OStreamWrapper osw( os );
	rapidjson::Writer<decltype( osw )> writer( osw );
	input.Accept( writer );
}
