#pragma once

#include "IProcessor.h"

class	EchoProc : public IProcessor::ReflectionImp<EchoProc>
{
public:

	virtual void ProcessJSON( ContextInfoSPtr& procInfo, JsonDOM& dom ) override;
};
