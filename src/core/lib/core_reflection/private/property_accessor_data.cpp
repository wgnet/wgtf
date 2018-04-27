#include "property_accessor_data.hpp"
#include "core_reflection/object/object_reference.hpp"

namespace wgt
{

namespace PropertyAccessorPrivate
{

//==============================================================================
Data::Data(Data & other)
	: object_(other.object_)
	, property_(other.property_)
	, path_(other.path_)
	, reference_(other.reference_)
{

}


//---------------------------------------------------------------------------
Data::Data(
	const char* path,
	const std::shared_ptr<ObjectReference>& reference)
	: reference_(reference)
	, path_(path)
	, object_(ObjectReference::asHandle(reference))
{

}

}

}