#include "IProcessor.h"

#include "WSSession.h"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>

void IProcessor::DispatchMsg(AsyncThreadPool& threadPool, const beast::websocket::opcode& op, WSSessionSPtr& session)
{
	auto& recvBuf = session->GetRecvBuf();

	std::string s;
	{//read buf
		auto cb = beast::consumed_buffers(recvBuf.data(), 0);
		s.reserve(boost::asio::buffer_size(cb));
		for (auto const& buffer : cb)
		{
			s.append(boost::asio::buffer_cast<const char*>(buffer), boost::asio::buffer_size(buffer));
		}
	}

	//clear buf
	recvBuf.consume(recvBuf.size());

	try
	{
		auto procInfo = std::make_shared<IProcessor::SProcInfo>(threadPool);

		std::istringstream is(s);
		boost::property_tree::json_parser::read_json(is, procInfo->Content_);
		procInfo->OpCode_ = op;
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
