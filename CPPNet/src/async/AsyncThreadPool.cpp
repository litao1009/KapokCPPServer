#include "AsyncThreadPool.h"

#include "Proactor.h"

#include <atomic>
#include <vector>
#include <thread>

class	AsyncThreadPool::Imp
{
public:

	std::vector<std::thread>	ThreadGroup_;
	Proactor					Proactor_;
	Listener					Listener_;

	std::atomic_bool	IsStart_{ false };
	std::atomic_bool	IsStop_{ false };
};

AsyncThreadPool::AsyncThreadPool():
	ImpUPtr_(std::make_unique<Imp>())
{}

AsyncThreadPool::~AsyncThreadPool()
{
	auto& imp_ = *ImpUPtr_;

	if( !imp_.IsStop_.load() )
	{
		imp_.Proactor_.Stop();

		for( auto& curThread : imp_.ThreadGroup_ )
		{
			curThread.join();
		}
	}
}

AsyncThreadPool::AsyncThreadPool(AsyncThreadPool && rhs) :ImpUPtr_(std::move(rhs.ImpUPtr_))
{
}

void AsyncThreadPool::Start(uint32_t ThreadNr)
{
	auto& imp_ = *ImpUPtr_;

	if( imp_.IsStart_.load() )
	{
		return;
	}

	imp_.ThreadGroup_.reserve(ThreadNr);

	// start threads
	for( auto i = 0U; i < ThreadNr; ++i )
	{
		imp_.ThreadGroup_.emplace_back(([&imp_]()
		{
			imp_.Listener_.OnStartThread_();

			imp_.Proactor_.Run();
		}));
	}

	imp_.IsStart_.store(true);
}

ErrCode AsyncThreadPool::Run()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Proactor_.Run();
}

void AsyncThreadPool::Join()
{
	auto& imp_ = *ImpUPtr_;

	for( auto& curThread : ImpUPtr_->ThreadGroup_ )
	{
		auto tid = curThread.get_id();
		curThread.join();
		
		imp_.Listener_.OnPostJoinThread_(tid);
	}

	imp_.Listener_.OnPostJoinAllThread_();
}

void AsyncThreadPool::Stop()
{
	auto& imp_ = *ImpUPtr_;

	imp_.Proactor_.Stop();

	imp_.IsStop_.store(true);
}


bool AsyncThreadPool::IsStop() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.IsStop_.load();
}


IOService& AsyncThreadPool::GetIOService()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Proactor_.GetIOService();
}

uint32_t AsyncThreadPool::GetThreadsSize() const
{
	return ImpUPtr_->ThreadGroup_.size();
}

void AsyncThreadPool::Post(Task task)
{
	auto& imp_ = *ImpUPtr_;

	imp_.Proactor_.GetIOService().post(task);
}

AsyncThreadPool::Listener& AsyncThreadPool::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}