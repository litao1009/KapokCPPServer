#pragma once

#include "ServerTestUnit.h"


class	PrepareSampleData_Protobuf : public ServerTestUnit::ReflectionImp<PrepareSampleData_Protobuf>
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	PrepareSampleData_Protobuf();

	~PrepareSampleData_Protobuf();

public:

	virtual void	Test();
};

