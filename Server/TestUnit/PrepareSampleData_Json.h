#pragma once

#include "ServerTestUnit.h"


class	PrepareSampleData_Json : public ServerTestUnit::ReflectionImp<PrepareSampleData_Json>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	PrepareSampleData_Json();

	~PrepareSampleData_Json();

public:

	virtual void	Test();
};

