#pragma once

#include "IProcessor.h"

class	RenderRequestProc : public IProcessor::ReflectionImp<RenderRequestProc>
{
public:

	virtual void Process(const beast::websocket::opcode& op, WSSessionSPtr& session, const Ptree& content) override;

};
