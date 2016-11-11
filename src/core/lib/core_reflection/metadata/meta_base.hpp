#ifndef METABASE_HPP
#define METABASE_HPP

/*
All reflected meta attribute classes should inherit from this class.
Details: Search for NGT Reflection System on the Wargaming Confluence
*/

#include "../reflected_object.hpp"
#include "../object_handle.hpp"
#include "core_reflection/reflection_dll.hpp"

namespace wgt
{
typedef ObjectHandleT<MetaBase> MetaHandle;

namespace MetaParamTypes
{
enum MetaParamType
{
	kUndefined,
	kBoolean,
	kInteger,
	kFloat,
	kString,
	kEnum,
	kHandle,
	kHandleList,
	kTable,
	kFunction,
	kImportName,
};
}

class REFLECTION_DLL MetaBase
{
	DECLARE_REFLECTED

public:
	MetaBase();
	virtual ~MetaBase();

private:
	MetaHandle next() const
	{
		return nextMetaData_;
	}
	void setNext(const MetaHandle& next) const
	{
		nextMetaData_ = next;
	}

	mutable MetaHandle nextMetaData_;

	friend REFLECTION_DLL const MetaHandle& operator+(const MetaHandle& left, const MetaHandle& right);
	friend REFLECTION_DLL MetaHandle findFirstMetaData(const TypeId& typeId, const MetaHandle& metaData,
	                                                   const IDefinitionManager& definitionManager);
};

REFLECTION_DLL const MetaHandle& operator+(const MetaHandle& left, const MetaHandle& right);
} // end namespace wgt
#endif
