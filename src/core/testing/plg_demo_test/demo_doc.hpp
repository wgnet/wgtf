#pragma once

#include "demo_objects.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_environment_system/i_env_system.hpp"
#include "core_viewport/viewport.hpp"

namespace wgt
{
class DemoDoc
	: public Viewport
	, public Depends< IEnvManager >
{
public:
	DemoDoc(const char* name, ObjectHandleT<DemoObjects> demo)
	    : Viewport(name, "DemoTest/Framebuffer.qml", demo->createScene(name))
	{
		Depends< IEnvManager >::get<IEnvManager>()->createNewEnvironment(name, this);
	}

	~DemoDoc()
	{
		Depends< IEnvManager >::get<IEnvManager>()->removeEnvironment(getId());
	}
};
}