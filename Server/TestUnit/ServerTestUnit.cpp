#include "ServerTestUnit.h"

void ServerTestUnit::Run()
{
	auto& genMap = GetGenMap();
	for ( auto& curValue : genMap )
	{
		auto testProc = curValue.second();
		testProc->Test();
	}
}
