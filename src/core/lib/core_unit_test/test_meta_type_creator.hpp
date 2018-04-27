#pragma once

#include "core_reflection/definition_manager.hpp"
#include "core_reflection/metadata/meta_type_creator.hpp"

namespace wgt
{
class TestMetaTypeCreator : public MetaTypeCreator
{
public:
    TestMetaTypeCreator()
	{
		interface_ = registerInterface<IMetaTypeCreator>(this);
	}

	virtual ~TestMetaTypeCreator()
	{
		deregisterInterface(interface_.get());
	}

private:
	InterfacePtr interface_ = nullptr;
};
}