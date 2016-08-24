#include "Proactor.h"

#include "ErrCodeUtil.h"

class	Proactor::Imp
{
public:

	IOService		IoService_;
	IOService::work	Work_{ IoService_ };
	Strand			Strand_{ IoService_ };
};

Proactor::Proactor():ImpUPtr_(std::make_unique<Imp>())
{

}

Proactor::~Proactor()
{

}

ErrCode Proactor::Run()
{
	auto& imp_ = *ImpUPtr_;

	boost::system::error_code ec;

	imp_.IoService_.run(ec);

	return ConvertBoostECToStdEC(ec);
}

void Proactor::Stop()
{
	auto& imp_ = *ImpUPtr_;

	imp_.IoService_.stop();
}

bool Proactor::IsStop() const
{
	auto& imp_ = *ImpUPtr_;

	return imp_.IoService_.stopped();
}

IOService& Proactor::GetIOService()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.IoService_;
}

Strand& Proactor::GetStrand()
{
	auto& imp_ = *ImpUPtr_;

	return imp_.Strand_;
}