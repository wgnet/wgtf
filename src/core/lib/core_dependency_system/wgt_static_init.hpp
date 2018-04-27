#ifndef WGT_STATIC_INIT_HPP
#define WGT_STATIC_INIT_HPP

#include "common_include/i_static_initializer.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_common/assert.hpp"
#include "core_common/holder_collection.hpp"
#include "core_common/tuple_helper.hpp"

namespace wgt
{
class WGTStaticExecutor
{
public:
	virtual ~WGTStaticExecutor() = default;

	WGTStaticExecutor(
		const IStaticInitalizer::ExecFunc& execFunc,
		const IStaticInitalizer::FiniFunc & finiFunc = [](){} );

protected:
	WGTStaticExecutor();
	void initFunc(
		const IStaticInitalizer::ExecFunc & execFunc,
		const IStaticInitalizer::FiniFunc & finiFunc);

private:
	std::shared_ptr< IStaticInitalizer::ExecFunc > execFunc_;
	std::shared_ptr< IStaticInitalizer::FiniFunc > finiFunc_;
};


template< typename First, typename ...Rest >
struct InterfaceCasterHelper
{
	static decltype( std::tuple_cat(InterfaceCasterHelper< First >::typeCaster(
		std::declval< IComponentContext & >(),
		std::declval< IComponentContextListener::InterfaceCaster * >(),
		std::declval< bool & >() ),
		InterfaceCasterHelper< Rest... >::typeCaster(
		std::declval< IComponentContext & >(),
		std::declval< IComponentContextListener::InterfaceCaster * >(),
		std::declval< bool & >() ) ) ) typeCaster(
		IComponentContext & context,
		IComponentContextListener::InterfaceCaster * caster, bool & success )
	{
		return
			std::tuple_cat(InterfaceCasterHelper< First >::typeCaster( context, caster, success ),
				InterfaceCasterHelper< Rest... >::typeCaster(context, caster, success));
	}
};

//==============================================================================
template< typename T >
struct InterfaceCasterHelper< T >
{
	static std::tuple< T > typeCaster(
		IComponentContext & context,
		IComponentContextListener::InterfaceCaster * caster,  bool & success )
	{
		typedef typename std::remove_reference< T >::type raw_type;
		static auto s_TypeId = TypeId::getType< raw_type >();
		raw_type * pInterface = caster ?
			static_cast< raw_type *>((*caster)(s_TypeId)) : nullptr;
		if (pInterface == nullptr)
		{
			pInterface = context.queryInterface< raw_type >();
		}
		if (pInterface)
		{
			success &= true;
			return std::forward_as_tuple< T >( *pInterface );
		}
		success = false;
		int dummyRef = 0;
		return std::forward_as_tuple< T >( reinterpret_cast< T >( dummyRef ) );
	}
};

//==============================================================================
template< typename First, typename ...Rest >
struct InterfaceMatcher
{
	static bool test( IComponentContextListener::InterfaceCaster & caster )
	{
		return InterfaceMatcher< First >::test( caster ) ||
			InterfaceMatcher< Rest... >::test( caster );
	}
};

//==============================================================================
template< typename T >
struct InterfaceMatcher< T >
{
	static bool test( IComponentContextListener::InterfaceCaster & caster)
	{
		typedef typename std::remove_reference< T >::type raw_type;
		static auto s_TypeId = TypeId::getType< raw_type >();
		return caster(s_TypeId) != nullptr;
	}
};

template< typename ... Args >
class WGTCallbackExecutorT
	: public WGTStaticExecutor
{
public:
	//--------------------------------------------------------------------------
	WGTCallbackExecutorT() {}


	//--------------------------------------------------------------------------
	template< typename T>
	WGTCallbackExecutorT(T initFunc)
	{
		initExecutor( initFunc );
	}


	//--------------------------------------------------------------------------
	template< typename T, typename U>
	WGTCallbackExecutorT(T initFunc, U finiFunc )
	{
		initExecutor( initFunc, finiFunc );
	}

	//--------------------------------------------------------------------------
	virtual ~WGTCallbackExecutorT()
	{
		impl_.reset();
	}

protected:
	//--------------------------------------------------------------------------
	void initExecutor( const std::function< void(Args...) > & execFunc )
	{
		initFunc([this, execFunc](IComponentContext & context)
		{
			impl_.reset(new WGTCallbackExecutorImpl(context, execFunc));
			impl_->connections_ += context.registerListener(*impl_);
		}, [this]()
		{
			impl_.reset();
		});
	}


	//--------------------------------------------------------------------------
	void initExecutor(
		const std::function< void(Args...) > & execFunc,
		const std::function< void(Args...) > & finiFunc)
	{
		initFunc([this, execFunc, finiFunc ](IComponentContext & context)
		{
			impl_.reset(new WGTCallbackExecutorImpl(context, execFunc, finiFunc));
			impl_->connections_ += context.registerListener(*impl_);
		}, [this]()
		{
			impl_.reset();
		});
	}


private:
	class WGTCallbackExecutorImpl
		: public IComponentContextListener
	{
	public:
		WGTCallbackExecutorImpl(
			IComponentContext & context,
			const std::function< void(Args...) > & initFunc )
			: context_( context )
			, inited_( false )
			, initFunc_( initFunc )
		{
		}

		//----------------------------------------------------------------------
		WGTCallbackExecutorImpl(
			IComponentContext & context,
			const std::function< void(Args...) > & initFunc,
			const std::function< void(Args...) > & finiFunc )
			: context_(context)
			, inited_(false)
			, initFunc_(initFunc)
			, finiFunc_(finiFunc)
		{
		}

		//----------------------------------------------------------------------
		~WGTCallbackExecutorImpl()
		{
			finiStaticInit( nullptr );
		}


		//----------------------------------------------------------------------
		void onInterfaceRegistered(InterfaceCaster& caster) override
		{
			if (inited_)
			{
				return;
			}
			bool success = true;
			auto dependencies
				= InterfaceCasterHelper< Args... >::typeCaster(
					context_, &caster, success);
			if (success == false)
			{
				return;
			}
			inited_ = true;
			apply_tuple(
				std::forward< decltype( initFunc_ )>( initFunc_ ),
				std::forward< decltype( dependencies ) >( dependencies ) );
		}

		//------------------------------------------------------------------
		void onInterfaceDeregistered(InterfaceCaster& caster) override
		{
			bool success = InterfaceMatcher< Args... >::test( caster );
			if (success == false)
			{
				return;
			}
			if (inited_ == false || finiFunc_ == nullptr)
			{
				inited_ = false;
				return;
			}
			finiStaticInit( &caster );
		}


		//----------------------------------------------------------------------
		void finiStaticInit( InterfaceCaster * caster )
		{
			if (inited_ == false || finiFunc_ == nullptr)
			{
				return;
			}
			bool success = true;
			auto dependencies
				= InterfaceCasterHelper< Args... >::typeCaster(
					context_, caster, success);
			if (success == false)
			{
				TF_ASSERT(false && "This should never happen!");
			}
			apply_tuple(
				std::forward< decltype(finiFunc_)>(finiFunc_),
				std::forward< decltype(dependencies) >(dependencies));
			inited_ = false;
		}

		bool inited_;
		const std::function< void(Args...) > initFunc_;
		const std::function< void(Args...) > finiFunc_;
		IComponentContext & context_;
		HolderCollection< IComponentContext::ConnectionHolder > connections_;
	};

	std::unique_ptr< WGTCallbackExecutorImpl > impl_;
};

template< typename T>
struct repack_args;

template< typename ...Args >
struct repack_args< std::tuple< Args... > >
{
	typedef WGTCallbackExecutorT< Args...> type;
};

} // end namespace wgt
#endif // WGT_STATIC_INIT_HPP
