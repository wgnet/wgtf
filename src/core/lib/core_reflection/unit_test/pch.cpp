#include "pch.hpp"

#include "reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"
#include "core_dependency_system/context_callback_helper.hpp"

namespace wgt
{

	struct ReflectionInit
		: public ContextCallBackHelper
	{
		ReflectionInit()
		{
			registerCallback([](IDefinitionManager & defManager)
			{
				ReflectionAutoRegistration::initAutoRegistration(defManager);
			});
		}
	};


	static ReflectionInit s_ReflectionIniter;
}