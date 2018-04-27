#pragma once

#include "core_object/object_manager.hpp"
#include "core_unit_test/test_global_context.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_object/object_reference.hpp"

namespace wgt
{
class TestObjectManager : public ObjectManager
{
public:
	TestObjectManager() : context_(this)
	{
		interface_ = registerInterface<IObjectManager>(this);
	}

	~TestObjectManager()
	{
		deregisterInterface(interface_.get());
	}

	virtual bool getContextObjects(std::vector<RefObjectId>& o_objects, IObjectManager* context = nullptr) const override
	{
		return ObjectManager::getContextObjects(o_objects, context_);
	}

	virtual bool registerContext(IObjectManager* context = nullptr) override
	{
		return ObjectManager::registerContext(context_);
	}

	virtual bool deregisterContext(IObjectManager* context = nullptr) override
	{
		return ObjectManager::deregisterContext(context_);
	}

	virtual bool saveObjects(ISerializer& serializer, IObjectManager* context = nullptr) override
	{
		return ObjectManager::saveObjects(serializer, context_);
	}

private:
	InterfacePtr interface_ = nullptr;
	IObjectManager* context_ = nullptr;
};
}