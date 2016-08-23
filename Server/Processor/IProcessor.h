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

	using	ContentType = rapidjson::Document;

	class	SProcInfo
	{
	public:

		SProcInfo(AsyncThreadPool& pool) :ThreadPool_(pool) {}

		MessagePtr			RawMsg_;
		WSPPSessionSPtr		Session_;
		ContentType			Content_;
		AsyncThreadPool&	ThreadPool_;
	};
	using	SProcInfoSPtr = std::shared_ptr<SProcInfo>;

public:

	static	void	DispatchMsg(AsyncThreadPool& threadPool, MessagePtr& msg, WSPPSessionSPtr& session);

	static	std::string	WriteJson(const ContentType& input);

	static	void		WriteJson( std::ostream& os, const ContentType& input );

public:

	virtual	void	Process(SProcInfoSPtr& procInfo) {}
};