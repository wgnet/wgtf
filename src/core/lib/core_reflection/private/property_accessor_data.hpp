#ifndef PROPERTY_ACCESSOR_DATA_HPP
#define PROPERTY_ACCESSOR_DATA_HPP

#include "core_dependency_system/depends.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"

namespace wgt
{

namespace PropertyAccessorPrivate
{

//==============================================================================
struct Data
	: Depends< IDefinitionManager, IObjectManager >
{
	//---------------------------------------------------------------------------
	Data(Data & other);


	//---------------------------------------------------------------------------
	Data::Data(
		const char* path,
		const std::shared_ptr<ObjectReference>& reference);

	IBasePropertyPtr property_;
	ObjectHandle object_;
	std::string path_;
	mutable ObjectHandle rootObject_;
	mutable std::string fullPath_;
	std::shared_ptr<ObjectReference> reference_;
	mutable std::shared_ptr<ObjectReference> rootReference_;
};

}

}

#endif //PROPERTY_ACCESSOR_DATA_HPP