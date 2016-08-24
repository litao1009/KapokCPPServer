#include "ErrCodeUtil.h"

#include <mutex>
#include <unordered_map>

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>

namespace
{
	std::once_flag	sOnceFlag;
}

class	BoostErrorCategoryWrapper : public std::error_category
{
	const boost::system::error_category&	BoostEC_;
	std::string								Name_{ "boost." };

public:

	class	Helper
	{
		using	ErrCategoryPtr = const boost::system::error_category*;
		using	CategoryList = std::unordered_map<ErrCategoryPtr, std::unique_ptr<BoostErrorCategoryWrapper>>;

		CategoryList		CategoryList_;
		boost::shared_mutex	Mutex_;

	public:

		static Helper&	GetInstance()
		{
			static	Helper sIns;
			return sIns;
		}

	public:

		std::error_category&	GetErrorCategory( const boost::system::error_category& boostErrCat )
		{
			{
				boost::shared_lock<boost::shared_mutex> slock( Mutex_ );
				auto itor = CategoryList_.find( &boostErrCat );
				if ( itor != CategoryList_.end() )
				{
					return *( itor->second );
				}
			}

			{
				boost::unique_lock<boost::shared_mutex> lock( Mutex_ );
				auto pair = CategoryList_.emplace( &boostErrCat, std::make_unique<BoostErrorCategoryWrapper>( boostErrCat ) );
				return *( pair.first->second );
			}
		}
	};

public:

	BoostErrorCategoryWrapper( const decltype( ( BoostEC_ ) )& ec ) :BoostEC_( ec )
	{
		std::call_once( sOnceFlag, Helper::GetInstance );

		Name_ += BoostEC_.name();
	}

public:

	virtual const char * name() const _NOEXCEPT override
	{
		return Name_.c_str();
	}

	virtual std::string message( int _Errval ) const override
	{
		return BoostEC_.message( _Errval );
	}

	virtual std::error_condition default_error_condition( int _Errval ) const _NOEXCEPT override
	{
		auto errCond = BoostEC_.default_error_condition( _Errval );

		return{ errCond.value(), Helper::GetInstance().GetErrorCategory( errCond.category() ) };
	}
};

std::error_code ConvertBoostECToStdEC( const boost::system::error_code& ec )
{
	auto ecValue = ec.value();
	auto& ecCategory = ec.category();

	if ( ecCategory == boost::system::generic_category() )
	{
		return{ ecValue, std::generic_category() };
	}

	return{ ecValue, BoostErrorCategoryWrapper::Helper::GetInstance().GetErrorCategory( ecCategory ) };
}