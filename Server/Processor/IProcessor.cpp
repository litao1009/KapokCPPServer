#include "IProcessor.h"

#include "WSPPSession.h"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>

void IProcessor::DispatchMsg(AsyncThreadPool& threadPool, MessagePtr& msg, WSPPSessionSPtr& session)
{
	try
	{
		auto procInfo = std::make_shared<IProcessor::SProcInfo>(threadPool);

		std::istringstream is(msg->get_payload());
		boost::property_tree::json_parser::read_json(is, procInfo->Content_);
		procInfo->RawMsg_ = msg;
		procInfo->Session_ = std::move(session);

		auto messageName = procInfo->Content_.get_optional<std::string>("MessageName");
		if (!messageName)
		{
			return;
		}

		auto proc = IProcessor::CreateByKey(*messageName);
		if (proc)
		{
			proc->Process(procInfo);
		}
	}
	catch (const std::exception&)
	{

	}
}

std::string IProcessor::WriteJson(const Ptree& input)
{
	std::stringstream ss;
	boost::property_tree::json_parser::write_json(ss, input);
	return ss.str();
}
