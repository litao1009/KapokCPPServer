#include "IProcessor.h"

#include "WSPPSession.h"

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <sstream>

void IProcessor::DispatchMsg(AsyncThreadPool& threadPool, MessagePtr& msg, WSPPSessionSPtr& session)
{
	try
	{
		auto procInfo = std::make_shared<IProcessor::SProcInfo>(threadPool);

		std::istringstream is(msg->get_payload());
		rapidjson::IStreamWrapper isw( is );
		procInfo->Content_.ParseStream( isw );

		msg->get_raw_payload().clear();
		msg->get_raw_payload().shrink_to_fit();

		procInfo->RawMsg_ = msg;
		procInfo->Session_ = std::move(session);

		if ( !procInfo->Content_.HasMember( "MessageName" ) )
		{
			return;
		}

		auto proc = IProcessor::CreateByKey( procInfo->Content_["MessageName"].GetString() );
		if (proc)
		{
			proc->Process(procInfo);
		}
	}
	catch (const std::exception&)
	{

	}
}

std::string IProcessor::WriteJson(const ContentType& input)
{
	std::stringstream ss;
	
	WriteJson( ss, input );

	return ss.str();
}

void IProcessor::WriteJson( std::ostream & os, const ContentType & input )
{
	rapidjson::OStreamWrapper osw( os );
	rapidjson::Writer<decltype( osw )> writer( osw );
	input.Accept( writer );
}
