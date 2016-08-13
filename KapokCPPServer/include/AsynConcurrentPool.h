#pragma once

#include "AsynConcurrentPoolFwd.h"
#include "Predef.h"

#include <functional>
#include <thread>

#include <boost/signals2.hpp>

class AsynConcurrentPool
{
	class	Imp;
	std::unique_ptr<Imp>	ImpUPtr_;

public:

	using	Task = std::function<void()>;

	class	Listener
	{
	public:

		boost::signals2::signal<void()>							OnStartThread_;
		boost::signals2::signal<void(const std::thread::id&)>	OnPostJoinThread_;
		boost::signals2::signal<void()>							OnPostJoinAllThread_;
	};

public:

	AsynConcurrentPool(uint32_t ThreadNr);

	~AsynConcurrentPool();

	AsynConcurrentPool(const AsynConcurrentPool&) = delete;

public:

	void				Start();

	ErrCode				Run();

	void				Join();

	void				Stop();

	bool				IsStop() const;

	IOService&			GetIOService();

	void				Post(Task task);

	uint32_t			GetThreadsSize() const;

	Listener&			GetListener();
};