#pragma once

#include "IProcessor.h"

class	RenderRequestProc : public IProcessor::ReflectionImp<RenderRequestProc>
{
public:

	virtual void Process(SProcInfoSPtr& procInfo) override;

};
