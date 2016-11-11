#ifndef LAMBDA_PROPERTY_HPP
#define LAMBDA_PROPERTY_HPP

#include "base_property.hpp"
#include "reflection_dll.hpp"
#include "metadata/meta_base.hpp"
#include "object_handle.hpp"

#include <functional>

namespace wgt
{
class REFLECTION_DLL LambdaProperty : public BaseProperty
{
	typedef BaseProperty base;

public:
	typedef std::function<Variant(const ObjectHandle& object, const IDefinitionManager& definitionManager)> Getter;

	typedef std::function<bool(const ObjectHandle& handle, const Variant& value,
	                           const IDefinitionManager& definitionManager)>
	Setter;

	LambdaProperty(const char* name, const TypeId& type, Getter getter, Setter setter, MetaHandle metaData);

	template <typename ObjectType, typename GetterT>
	static IBasePropertyPtr create(const char* name, GetterT&& getter, MetaHandle metaData)
	{
		typedef typename std::decay<typename std::result_of<GetterT(ObjectType*)>::type>::type property_type;

		return std::make_shared<LambdaProperty>(name, TypeId::getType<property_type>(),
		                                        wrapGetter<ObjectType>(std::forward<GetterT>(getter)), Setter(),
		                                        std::move(metaData));
	}

	template <typename ObjectType, typename GetterT, typename SetterT>
	static IBasePropertyPtr create(const char* name, GetterT&& getter, SetterT&& setter, MetaHandle metaData)
	{
		typedef typename std::decay<SetterT>::type setter_type;
		typedef typename std::decay<typename std::result_of<GetterT(ObjectType*)>::type>::type property_type;

		return std::make_shared<LambdaProperty>(
		name, TypeId::getType<property_type>(), wrapGetter<ObjectType>(std::forward<GetterT>(getter)),
		SetterWrapper<setter_type, ObjectType, property_type>::wrap(std::forward<SetterT>(setter)),
		std::move(metaData));
	}

	MetaHandle getMetaData() const override;
	bool readOnly() const override;
	bool isMethod() const override;
	bool isValue() const override;

	bool set(const ObjectHandle& handle, const Variant& value,
	         const IDefinitionManager& definitionManager) const override;

	Variant get(const ObjectHandle& handle, const IDefinitionManager& definitionManager) const override;

private:
	Getter getter_;
	Setter setter_;
	MetaHandle metaData_;

	template <typename ObjectType, typename GetterT>
	static Getter wrapGetter(GetterT&& getter)
	{
		return [getter](const ObjectHandle& object, const IDefinitionManager& definitionManager) -> Variant {
			auto pObject = reflectedCast<ObjectType>(object.data(), object.type(), definitionManager);
			if (pObject)
			{
				return getter(pObject);
			}
			else
			{
				return Variant();
			}
		};
	}

	template <typename SetterType, typename ObjectType, typename PropertyType,
	          bool = std::is_void<typename std::result_of<SetterType(ObjectType*, const PropertyType&)>::type>::value>
	struct NonNullSetterWrapper;

	template <typename SetterType, typename ObjectType, typename PropertyType>
	struct NonNullSetterWrapper<SetterType, ObjectType, PropertyType, false>
	{
		template <typename T>
		static Setter wrap(T&& setter)
		{
			return [setter](const ObjectHandle& object, const Variant& value,
			                const IDefinitionManager& definitionManager) -> bool {
				auto pObject = reflectedCast<ObjectType>(object.data(), object.type(), definitionManager);
				if (!pObject)
				{
					return false;
				}

				bool result = false;
				value.visit<PropertyType>([&](const PropertyType& v) { result = setter(pObject, v); });

				return result;
			};
		}
	};

	template <typename SetterType, typename ObjectType, typename PropertyType>
	struct NonNullSetterWrapper<SetterType, ObjectType, PropertyType, true>
	{
		template <typename T>
		static Setter wrap(T&& setter)
		{
			return [setter](const ObjectHandle& object, const Variant& value,
			                const IDefinitionManager& definitionManager) -> bool {
				auto pObject = reflectedCast<ObjectType>(object.data(), object.type(), definitionManager);
				if (!pObject)
				{
					return false;
				}

				return value.visit<PropertyType>([&](const PropertyType& v) { setter(pObject, v); });
			};
		}
	};

	template <typename SetterType, typename ObjectType, typename PropertyType>
	struct SetterWrapper : public NonNullSetterWrapper<SetterType, ObjectType, PropertyType>
	{
	};

	template <typename ObjectType, typename PropertyType>
	struct SetterWrapper<std::nullptr_t, ObjectType, PropertyType>
	{
		static Setter wrap(std::nullptr_t)
		{
			return Setter();
		}
	};
};
}

#endif
