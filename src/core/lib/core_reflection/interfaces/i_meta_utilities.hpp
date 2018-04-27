#ifndef I_META_UTILITIES_HPP
#define I_META_UTILITIES_HPP

#include <functional>

namespace wgt
{

class IBaseProperty;
class IClassDefinition;
class IDefinitionManager;
class MetaData;
class ObjectHandle;
class PropertyAccessor;
class TypeId;

//==============================================================================
class REFLECTION_DLL IMetaUtilities
{
public:
	using MetaDataCallback = std::function<void(const ObjectHandle&)>;

	virtual ~IMetaUtilities() {} 

	virtual ObjectHandle findFirstMetaData(
		const TypeId& , const MetaData & , const IDefinitionManager & ) = 0;

	virtual ObjectHandle findFirstMetaData(
		const IClassDefinition& , const MetaData &, const IDefinitionManager & ) = 0;

	virtual ObjectHandle findFirstMetaData(
		const TypeId& , const PropertyAccessor&, const IDefinitionManager &) = 0;

	virtual ObjectHandle findFirstMetaData(
		const TypeId& , const IBaseProperty&, const IDefinitionManager &) = 0;

	virtual ObjectHandle findFirstMetaData(
		const TypeId& , const IClassDefinition&, const IDefinitionManager &) = 0;

	virtual void forEachMetaData(
		const IClassDefinition&, const MetaData &, const IDefinitionManager &, MetaDataCallback callback) = 0;

	virtual void forEachMetaData(
		const TypeId& , const MetaData &, const IDefinitionManager &, MetaDataCallback ) = 0;

	virtual MetaData&& add(MetaData&& left, MetaData&& right) = 0;
	virtual void add(MetaData& left, MetaData&& right) = 0;
};

} // end namespace wgt
#endif // I_META_UTILITIES_HPP
