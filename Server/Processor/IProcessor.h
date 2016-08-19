#pragma once

#include "IReflection.h"

#include "WSPredef.h"
#include "WSSessionFwd.h"

#include <boost/property_tree/ptree.hpp>

class	IProcessor : public IReflection<IProcessor>
{
public:

	virtual ~IProcessor() {}

public:

	using	Ptree = boost::property_tree::ptree;

public:

	static	void	DispatchMsg(const beast::websocket::opcode& op, WSSessionSPtr& session);

public:

	virtual	void	Process(const beast::websocket::opcode& op, WSSessionSPtr& session, const Ptree& content) {}
};