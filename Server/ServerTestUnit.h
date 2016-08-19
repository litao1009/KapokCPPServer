#pragma once

#include "IReflection.h"

class	ServerTestUnit : public IReflection<ServerTestUnit>
{
public:

	virtual ~ServerTestUnit() {}

public:

	static	void	Run();

public:

	virtual void	Test() {}
};
