#pragma once

#include "IReflection.h"

#include "WSPPPredef.h"
#include "WSPPSession.h"
#include "AsyncThreadPool.h"

#include "rapidjson/document.h"

class	IProcessor : public IReflection<IProcessor>
{
public:

	virtual ~IProcessor() {}

public:

	using	JsonDOM = rapidjson::Document;

	class	ContextInfo
	{
	public:

		ContextInfo(AsyncThreadPool& pool) :ThreadPool_(pool) {}

	public:

		WSPPSessionSPtr		Session_;
		AsyncThreadPool&	ThreadPool_;
	};
	using	ContextInfoSPtr = std::shared_ptr<ContextInfo>;

public:

	static	void	DispatchMsg(AsyncThreadPool& threadPool, MessagePtr& msg, WSPPSessionSPtr& session);

public:

	static	std::string	WriteJson(const JsonDOM& input);

	static	void		WriteJson( std::ostream& os, const JsonDOM& input );

public:

	virtual	void	ProcessJSON(ContextInfoSPtr& procInfo, JsonDOM& dom) {}

	virtual	void	ProcessProtobuf( ContextInfoSPtr& procInfo, std::string& content ) {}
};