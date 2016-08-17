#pragma once

#include "Predef.h"

#include <boost/asio.hpp>

class Proactor
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	Proactor();

	~Proactor();

	Proactor(const Proactor&) = delete;

public:

	ErrCode			Run();

	void			Stop();

	bool			IsStop() const;

	IOService&		GetIOService();

	Strand&			GetStrand();
};