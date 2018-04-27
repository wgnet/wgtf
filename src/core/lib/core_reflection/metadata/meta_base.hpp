#ifndef METABASE_HPP
#define METABASE_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_reflection/interfaces/i_meta_utilities.hpp"
#include "core_object/i_managed_object.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_object/object_handle_cast_utils.hpp"

namespace wgt
{
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

struct MetaDataStorage;

//==============================================================================
/**
* All reflected meta attribute classes should be stored in a MetaData
* Holds object storage and a MetaHandle for MetaData
* @note See meta_type_creator.cpp for examples
*/
class REFLECTION_DLL MetaData
{
public:
    template<typename T, typename... Args>
    static MetaData create(Args&&... args)
    {
        ManagedObjectPtr object = ManagedObject<T>::make_unique(std::forward<Args>(args)...);
        return MetaData(std::move(object), object->getHandle());
    }

	MetaData(const std::nullptr_t& = nullptr);
	MetaData(MetaData&& rhs);
	~MetaData();

	MetaData& operator=(MetaData&& rhs);

	bool operator==(const std::nullptr_t&) const;
	bool operator!=(const std::nullptr_t&) const;
	const ObjectHandle & getHandle() const;

	static IMetaUtilities & getMetaUtils();
private:
	MetaData(ManagedObjectPtr obj, ObjectHandle & handle);

	const MetaData & next() const;
	void setNext(MetaData next) const;
    MetaData(const MetaData& rhs) = delete;
    MetaData& operator=(const MetaData& rhs) = delete;

	std::unique_ptr< MetaDataStorage > storage_;
	friend class MetaUtilities;
};

//==============================================================================
template <class T>
ObjectHandleT<T> CastMetaData(const ObjectHandle & metaData, const IDefinitionManager& definitionManager)
{
	auto root = reflectedRoot(metaData, definitionManager);
	return reflectedCast<T>(root, definitionManager);
}

//==============================================================================
template <class T>
ObjectHandleT<T> findFirstMetaData(const MetaData & metaData, const IDefinitionManager& definitionManager)
{
	auto meta =
		MetaData::getMetaUtils().findFirstMetaData(TypeId::getType<T>(), metaData, definitionManager);
	return CastMetaData<T>(meta, definitionManager);
}

//==============================================================================
template <class T>
ObjectHandleT<T> findFirstMetaData(const PropertyAccessor& accessor, const IDefinitionManager& definitionManager)
{
	auto meta =
		MetaData::getMetaUtils().findFirstMetaData(TypeId::getType<T>(), accessor, definitionManager );
	return CastMetaData<T>(meta, definitionManager);
}

//==============================================================================
template <class T>
ObjectHandleT<T> findFirstMetaData(const IBaseProperty& pProperty, const IDefinitionManager& definitionManager)
{
	auto meta =
		MetaData::getMetaUtils().findFirstMetaData(TypeId::getType<T>(), pProperty, definitionManager);
	return CastMetaData<T>(meta, definitionManager);
}

//==============================================================================
template <class T>
ObjectHandleT<T> findFirstMetaData(const IClassDefinition& definition, const IDefinitionManager& definitionManager)
{
	auto meta =
		MetaData::getMetaUtils().findFirstMetaData(TypeId::getType<T>(), definition, definitionManager);
	return CastMetaData<T>(meta, definitionManager);
}

//==============================================================================
template <class T>
void forEachMetaData(const PropertyAccessor & accessor, const IDefinitionManager& definitionManager,
	std::function<void(const ObjectHandleT<T>&)> callback)
{
	auto func = [&](const ObjectHandle& handle)
	{
		callback(reinterpretCast<T>(handle));
	};
	const auto& typeId = TypeId::getType<T>();
	MetaData::getMetaUtils().forEachMetaData( typeId, accessor.getMetaData(), definitionManager, func );
}

//==============================================================================
template <class T>
void forEachMetaData(const MetaData & metaData, const IDefinitionManager& definitionManager,
	std::function<void(const ObjectHandleT<T>&)> callback)
{
	auto func = [&](const ObjectHandle& handle){
		callback(reinterpretCast<T>(handle));
	};
	const auto& typeId = TypeId::getType<T>();
	MetaData::getMetaUtils().forEachMetaData( typeId, metaData, definitionManager, func );
}

//==============================================================================
template <class T>
void forEachMetaData(const IClassDefinition& definition, const IDefinitionManager& definitionManager,
	std::function<void(const ObjectHandleT<T>&)> callback)
{
	auto func = [&](const ObjectHandle& handle){
		callback(reinterpretCast<T>(handle));
	};
	const auto& typeId = TypeId::getType<T>();
	MetaData::getMetaUtils().forEachMetaData( typeId, definition, definitionManager, func );
}

//--------------------------------------------------------------------------
template <class T>
void forEachMetaData(const IBaseProperty& pProperty, const IDefinitionManager & definitionManager,
	std::function<void(const ObjectHandleT<T>&)> callback)
{
	auto func = [&](const ObjectHandle& handle){
		callback(reinterpretCast<T>(handle));
	};
	auto && metaData = pProperty.getMetaData();
	const auto& typeId = TypeId::getType<T>();
	MetaData::getMetaUtils().forEachMetaData( typeId, metaData, definitionManager, func );
}

//==============================================================================
inline MetaData&& operator+(MetaData&& left, MetaData&& right)
{
	return MetaData::getMetaUtils().add(std::move(left), std::move(right));
}

//==============================================================================
inline void operator+=(MetaData& left, MetaData&& right)
{
	MetaData::getMetaUtils().add(left, std::move(right));
}

} // end namespace wgt
#endif
