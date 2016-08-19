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
	class ReflectionImp : public BaseT
	{
	public:

		using	ReflectionType = ReflectionImp<DerivedT>;

		static decltype(auto)	Factory()
		{
			return std::make_shared<DerivedT>();
		}

	private:

		class	Context
		{
		public:

			Context(const std::string& key)
			{
				BaseT::Register(key, std::bind(DerivedT::Factory));
			}

			Context():Context(typeid(DerivedT).name())
			{}
		};

		static	Context	sContext;

		using	ContextType = Context;
	};

public:

	static	decltype(auto)	CreateByClassName(const std::string& className)
	{
		return _Create("class " + className);
	}

	static	decltype(auto)	CreateByKey(const std::string& key)
	{
		return _Create(key);
	}

	static	void Register(const std::string& key, const ClassGen& genFtr)
	{
		auto& genMap_ = _GetGenMap();

		genMap_.emplace(key, genFtr);
	}

private:

	static	BaseSPtr	_Create(const std::string& key)
	{
		auto& genMap_ = _GetGenMap();

		auto itor = genMap_.find(key);
		if (itor != genMap_.end())
		{
			return itor->second();
		}

		return {};
	}

	static	decltype(auto)	_GetGenMap()
	{
		static	std::map<std::string, ClassGen> genMap;
		return (genMap);
	}

protected:

	static	decltype(auto)	GetGenMap()
	{
		const auto& map = _GetGenMap();
		return (map);
	}
};

#define IMPLEMNET_REFLECTION(DerivedT)	\
	DerivedT::Context	DerivedT::ReflectionType::sContext;

#define IMPLEMNET_REFLECTION_WITH_KEY(DerivedT, key)	\
	DerivedT::Context	DerivedT::ReflectionType::sContext(#key);