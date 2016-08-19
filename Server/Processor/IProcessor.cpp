#include "IProcessor.h"

#include "WSSession.h"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>

void IProcessor::DispatchMsg(const beast::websocket::opcode& op, WSSessionSPtr& session)
{
	auto& recvBuf = session->GetRecvBuf();

	std::string s;
	{
		auto cb = beast::consumed_buffers(recvBuf.data(), 0);
		s.reserve(boost::asio::buffer_size(cb));
		for (auto const& buffer : cb)
		{
			s.append(boost::asio::buffer_cast<const char*>(buffer), boost::asio::buffer_size(buffer));
		}
	}

	recvBuf.consume(recvBuf.size());

	try
	{
		std::istringstream is(s);
		Ptree pt;
		boost::property_tree::json_parser::read_json(is, pt);

		auto messageName = pt.get_optional<std::string>("MessageName");
		if (!messageName)
		{
			return;
		}

		auto proc = IProcessor::CreateByKey(*messageName);
		if (proc)
		{
			proc->Process(op, session, pt);
		}
	}
	catch (const std::exception&)
	{

	}
}
