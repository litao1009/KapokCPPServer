#pragma once

#include "IReflection.h"

#include "WSPredef.h"
#include "WSSession.h"
#include "AsyncThreadPool.h"

#include <boost/property_tree/ptree.hpp>

class	IProcessor : public IReflection<IProcessor>
{
public:

	virtual ~IProcessor() {}

public:

	using	Ptree = boost::property_tree::ptree;

	class	SProcInfo
	{
	public:

		SProcInfo(AsyncThreadPool& pool) :ThreadPool_(pool) {}

		beast::websocket::opcode	OpCode_;
		WSSessionSPtr				Session_;
		Ptree						Content_;
		AsyncThreadPool&			ThreadPool_;
	};
	using	SProcInfoSPtr = std::shared_ptr<SProcInfo>;

public:

	static	void	DispatchMsg(AsyncThreadPool& threadPool, const beast::websocket::opcode& op, WSSessionSPtr& session);

	static	std::string	WriteJson(const Ptree& input);

public:

	virtual	void	Process(SProcInfoSPtr& procInfo) {}
};