#include "AsynConcurrentPool.h"

#include "Proactor.h"

#include <atomic>
#include <vector>
#include <thread>

class	AsynConcurrentPool::Imp
{
public:

	std::vector<std::thread>	ThreadGroup_;
	uint32_t					ThreadNr_{};
	Proactor					Proactor_;
	Listener					Listener_;

	std::atomic_bool	IsStart_{ false };
	std::atomic_bool	IsStop_{ false };
};

AsynConcurrentPool::AsynConcurrentPool(uint32_t ThreadNr):
	ImpUPtr_(std::make_unique<Imp>())
{
	ImpUPtr_->ThreadNr_ = ThreadNr;
}

AsynConcurrentPool::~AsynConcurrentPool()
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

void AsynConcurrentPool::Start()
{
	auto& imp_ = *ImpUPtr_;

	if( imp_.IsStart_.load() )
	{
		return;
	}

	// start threads
	for( auto i = 0U; i < ImpUPtr_->ThreadNr_; ++i )
	{
		ImpUPtr_->ThreadGroup_.emplace_back(([&imp_]()
		{
			imp_.Listener_.OnStartThread_();

			imp_.Proactor_.Run();
		}));
	}

	ImpUPtr_->IsStart_.store(true);
}

ErrCode AsynConcurrentPool::Run()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Proactor_.Run();
}

void AsynConcurrentPool::Join()
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

void AsynConcurrentPool::Stop()
{
	auto& imp_ = *ImpUPtr_;

	imp_.Proactor_.Stop();

	imp_.IsStop_.store(true);
}


bool AsynConcurrentPool::IsStop() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.IsStop_.load();
}


IOService& AsynConcurrentPool::GetIOService()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Proactor_.GetIOService();
}

uint32_t AsynConcurrentPool::GetThreadsSize() const
{
	return ImpUPtr_->ThreadGroup_.size();
}

void AsynConcurrentPool::Post(Task task)
{
	auto& imp_ = *ImpUPtr_;

	imp_.Proactor_.GetIOService().post(task);
}

AsynConcurrentPool::Listener& AsynConcurrentPool::GetListener()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Listener_;
}