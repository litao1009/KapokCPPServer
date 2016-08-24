#pragma once

#include "IProcessor.h"

class	RenderRequestProc : public IProcessor::ReflectionImp<RenderRequestProc>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	RenderRequestProc();

	~RenderRequestProc();

public:

	virtual void ProcessJSON( ContextInfoSPtr& procInfo, JsonDOM& dom ) override;

};
