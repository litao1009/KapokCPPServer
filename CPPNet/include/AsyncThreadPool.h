#pragma once

#include "Predef.h"

#include <functional>
#include <thread>

#include <boost/signals2.hpp>

class AsyncThreadPool
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

	AsyncThreadPool();

	~AsyncThreadPool();

	AsyncThreadPool(const AsyncThreadPool&) = delete;

	AsyncThreadPool(AsyncThreadPool&& rhs);

public:

	void				Start(uint32_t ThreadNr);

	ErrCode				Run();

	void				Join();

	void				Stop();

	bool				IsStop() const;

	IOService&			GetIOService();

	void				Post(Task task);

	uint32_t			GetThreadsSize() const;

	Listener&			GetListener();
};