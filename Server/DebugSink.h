#pragma once

#include <memory>

class DebugSink
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	DebugSink();

	~DebugSink();

	DebugSink( const DebugSink& ) = delete;
};