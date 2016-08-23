#pragma once

#include "ServerTestUnit.h"


class	PrepareSampleData : public ServerTestUnit::ReflectionImp<PrepareSampleData>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	PrepareSampleData();

	~PrepareSampleData();

public:

	virtual void	Test();
};

