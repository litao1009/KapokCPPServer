#pragma once

#include <memory>

class	KapokServer
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	KapokServer();

	~KapokServer();

	KapokServer(const KapokServer&) = delete;

public:

	void	Start();

	void	Stop();

	void	Join();
};
