#ifndef DEPENDS_HPP
#define DEPENDS_HPP

#include <cassert>
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_variant/type_id.hpp"

#define INTERFACE_REQUEST(type, var, holder, retOnFalse) \
	type* var##Pointer = holder.get<type>();             \
	if (var##Pointer == nullptr)                         \
		return retOnFalse;                               \
	type& var = *var##Pointer;

namespace wgt
{
class DummyDependsType
{
};

//==============================================================================
// Change this to support as many classes as we need to inherit, until we have
// support for variadic templates
//==============================================================================

template <typename T1, typename T2 = DummyDependsType, typename T3 = DummyDependsType, typename T4 = DummyDependsType,
          typename T5 = DummyDependsType, typename T6 = DummyDependsType, typename T7 = DummyDependsType,
          typename T8 = DummyDependsType, typename T9 = DummyDependsType>
class DependsImpl : public DependsImpl<T2, T3, T4, T5, T6, T7, T8, T9>
{
protected:
	typedef DependsImpl<T2, T3, T4, T5, T6, T7, T8, T9> Base;

	DependsImpl() : pValue_(nullptr)
	{
	}

	DependsImpl& operator=(const DependsImpl& that)
	{
		pValue_ = that.pValue_;

		Base::operator=(that);

		return *this;
	}

	template <typename T, bool = false>
	struct GetHelper
	{
		//--------------------------------------------------------------------------
		template <typename Source>
		static T* get(IComponentContext& context, Source* pValue, const DependsImpl& pThis)
		{
			return pThis.Base::template get<T>(context);
		}
	};

	template <bool _dummy>
	struct GetHelper<T1, _dummy>
	{
		//--------------------------------------------------------------------------
		template <typename Source>
		static T1* get(IComponentContext& context, Source* pValue, const DependsImpl& pThis)
		{
			return pValue;
		}
	};

	//--------------------------------------------------------------------------
	template <typename T>
	T* get(IComponentContext& context) const
	{
		return GetHelper<T>::get(context, pValue_, *this);
	}

	//--------------------------------------------------------------------------
	template <typename T>
	void get(IComponentContext& context, std::vector<T*>& interfaces) const
	{
		if (std::is_same<T1, T>::value)
		{
			context.queryInterface<T>(interfaces);
			return;
		}
		return Base::template get<T>(context, interfaces);
	}

	//--------------------------------------------------------------------------
	void onInterfaceRegistered(IComponentContextListener::InterfaceCaster& caster)
	{
		static auto s_TypeId = TypeId::getType<T1>();
		T1* pInterface = static_cast<T1*>(caster(s_TypeId));
		if (pInterface)
		{
			pValue_ = pInterface;
			return;
		}
		Base::onInterfaceRegistered(caster);
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered(IComponentContextListener::InterfaceCaster& caster)
	{
		static auto s_TypeId = TypeId::getType<T1>();
		T1* pInterface = static_cast<T1*>(caster(s_TypeId));
		if (pInterface && pInterface == pValue_)
		{
			pValue_ = nullptr;
			return;
		}
		Base::onInterfaceDeregistered(caster);
	}

	void initValue(IComponentContext& context)
	{
		pValue_ = context.queryInterface<T1>();
		Base::initValue(context);
	}

private:
	T1* pValue_;
};

//==============================================================================
// Handle the empty type
//==============================================================================
template <>
class DependsImpl<DummyDependsType>
{
protected:
	DependsImpl& operator=(const DependsImpl& that)
	{
		// Do nothing
		return *this;
	}

	//--------------------------------------------------------------------------
	template <typename T>
	T* get(IComponentContext& context) const
	{
		return nullptr;
	}

	//--------------------------------------------------------------------------
	template <typename T>
	void get(IComponentContext& context, std::vector<T*>&) const
	{
		// Do nothing
	}

	//--------------------------------------------------------------------------
	void onInterfaceRegistered(IComponentContextListener::InterfaceCaster&)
	{
		// Do nothing
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered(IComponentContextListener::InterfaceCaster&)
	{
		// Do nothing
	}

	void initValue(IComponentContext& context)
	{
		// Do nothing
	}
};

//==============================================================================
// Change this to support as many classes as we need to inherit, until we have
// support for variadic templates
//==============================================================================
template <typename T1, typename T2 = DummyDependsType, typename T3 = DummyDependsType, typename T4 = DummyDependsType,
          typename T5 = DummyDependsType, typename T6 = DummyDependsType, typename T7 = DummyDependsType,
          typename T8 = DummyDependsType, typename T9 = DummyDependsType>
class Depends : public IComponentContextListener, private DependsImpl<T1, T2, T3, T4, T5, T6, T7, T8, T9>
{
	typedef DependsImpl<T1, T2, T3, T4, T5, T6, T7, T8, T9> Base;

public:
	Depends(IComponentContext& context) : context_(&context)
	{
		context_->registerListener(*this);

		// call init after registration to avoid races
		Base::initValue(*context_);
	}

	Depends(const Depends& that) : context_(that.context_)
	{
		context_->registerListener(*this);
		Base::operator=(that);
	}

	//--------------------------------------------------------------------------
	~Depends()
	{
		context_->deregisterListener(*this);
	}

	Depends& operator=(const Depends& that)
	{
		if (context_ != that.context_)
		{
			context_->deregisterListener(*this);
			context_ = that.context_;
			context_->registerListener(*this);
		}

		Base::operator=(that);

		return *this;
	}

	//--------------------------------------------------------------------------
	template <typename T>
	T* get() const
	{
		return Base::template get<T>(*context_);
	}

	//--------------------------------------------------------------------------
	template <typename T>
	void get(std::vector<T*>& interfaces) const
	{
		Base::template get<T>(*context_, interfaces);
	}

	operator IComponentContext&()
	{
		return *context_;
	}

private:
	//--------------------------------------------------------------------------
	void onInterfaceRegistered(InterfaceCaster& caster) override
	{
		Base::onInterfaceRegistered(caster);
	}

	//--------------------------------------------------------------------------
	void onInterfaceDeregistered(InterfaceCaster& caster) override
	{
		Base::onInterfaceDeregistered(caster);
	}

	IComponentContext* context_;
};
} // end namespace wgt
#endif // DEPENDS_HPP
