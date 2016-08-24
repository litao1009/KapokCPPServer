#pragma once

#include "IProcessor.h"

class	RenderRequestProc : public IProcessor::ReflectionImp<RenderRequestProc>
{
public:

	virtual void ProcessJSON( ContextInfoSPtr& procInfo, JsonDOM& dom ) override;

};
