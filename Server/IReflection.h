#pragma once

#include <functional>
#include <string>
#include <map>
#include <memory>

template<typename BaseT>
class	IReflection
{
protected:

	using	BaseSPtr = std::shared_ptr<BaseT>;
	using	ClassGen = std::function<BaseSPtr()>;

public:

	template<typename DerivedT>
	class IDerived : public BaseT
	{
	public:

		using	IBase = BaseT;
		using	ID = IDerived<DerivedT>;

		static BaseSPtr	Factory()
		{
			return std::make_shared<DerivedT>();
		}

	protected:

		class	Context
		{
		public:

			Context()
			{
				BaseT::Register(typeid(DerivedT).name(), std::bind(DerivedT::Factory));
			}
		};

		static	Context	sContext;
	};

public:

	static	BaseSPtr	Create(const std::string& className)
	{
		auto& genMap_ = _GetGenMap();

		auto itor = genMap_.find("class " + className);
		if ( itor != genMap_.end() )
		{
			return itor->second();
		}

		return nullptr;
	}

	static	void Register(const std::string& className, const ClassGen& genFtr)
	{
		auto& genMap_ = _GetGenMap();

		genMap_.emplace(className, genFtr);
	}

private:

	typedef	std::map<std::string, ClassGen>	GenMapType;
	static	GenMapType&	_GetGenMap()
	{
		static	GenMapType genMap;
		return genMap;
	}
};

#define IMPLEMNET_REFLECTION(DerivedT)	\
	DerivedT::Context	IReflection<DerivedT::IBase>::IDerived<DerivedT>::sContext;