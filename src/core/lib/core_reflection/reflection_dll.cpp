#include "reflection_dll.hpp"

#include "core_common/assert.hpp"
#include "core_dependency_system/context_callback_helper.hpp"
#include "metadata/meta_expose.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

namespace wgt
{

namespace ReflectionShared
{

struct ReflectionContextHolder
{
	ReflectionContextHolder(IComponentContext & context)
		: context_(context)
	{
	}

	IComponentContext & context_;
	IDefinitionManager * definitionManager_ = nullptr;
};

struct DeferedStaticCreator
{
	DeferedStaticCreator(
		const std::shared_ptr< IStaticInitalizer::ExecFunc > initFunc,
		const std::shared_ptr< IStaticInitalizer::FiniFunc > finiFunc)
		: initFunc_(initFunc)
		, finiFunc_(finiFunc)
	{
	}

	const std::shared_ptr< IStaticInitalizer::ExecFunc > initFunc_;
	const std::shared_ptr< IStaticInitalizer::FiniFunc > finiFunc_;
};

static std::unique_ptr< ReflectionContextHolder >	s_Holder;
static std::vector< DeferedStaticCreator >			s_StaticCreators;

//------------------------------------------------------------------------------
void deferredRegisterStatic(IComponentContext & context)
{
	static IStaticInitalizer* staticInitializer
		= context.queryInterface<IStaticInitalizer>();
	for (auto & staticCreator : s_StaticCreators)
	{
		staticInitializer->registerStaticInitializer(
				staticCreator.initFunc_, staticCreator.finiFunc_);
		staticInitializer->initStatics(context);
	}
	s_StaticCreators.clear();
}

//------------------------------------------------------------------------------
void initContext(IComponentContext & context )
{
	if (s_Holder != nullptr)
	{
		return;
	}
	s_Holder = std::make_unique< ReflectionContextHolder >(context);
	deferredRegisterStatic(context);
}


//------------------------------------------------------------------------------
void initDefinitionManager(IDefinitionManager & defManager)
{
	s_Holder->definitionManager_ = &defManager;
	ReflectionAutoRegistration::initAutoRegistration(defManager);
}

//------------------------------------------------------------------------------
IComponentContext & getComponentContext()
{
	TF_ASSERT(s_Holder != nullptr);
	return s_Holder->context_;
}

//------------------------------------------------------------------------------
IDefinitionManager & getDefinitionManager()
{
	assert(s_Holder != nullptr);
	assert(s_Holder->definitionManager_ != nullptr);
	return *s_Holder->definitionManager_;
}

} //end namespace ReflectionShared

//------------------------------------------------------------------------------
void registerStatic(
	const std::shared_ptr< IStaticInitalizer::ExecFunc > & initFunc,
	const std::shared_ptr< IStaticInitalizer::FiniFunc > & finiFunc)
{
	ReflectionShared::s_StaticCreators.emplace_back(
		ReflectionShared::DeferedStaticCreator(initFunc, finiFunc));
}

} //end namespace wgt
