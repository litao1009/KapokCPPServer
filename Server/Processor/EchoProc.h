#pragma once

#include "IProcessor.h"

class	EchoProc : public IProcessor::ReflectionImp<EchoProc>
{
public:

	virtual void Process(SProcInfoSPtr& procInfo) override;
};
