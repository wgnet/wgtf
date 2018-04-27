#ifndef META_UTILITIES_HPP
#define META_UTILITIES_HPP

#include "core_common/assert.hpp"
#include "core_reflection/interfaces/i_meta_utilities.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/property_accessor.hpp"

namespace wgt
{

//==============================================================================
class MetaUtilities
	: public IMetaUtilities
{
public:
	ObjectHandle findFirstMetaData(
		const TypeId& typeId, const MetaData & metaData,
		const IDefinitionManager & definitionManager) override
	{
		auto targetDefinition = definitionManager.getDefinition(typeId.getName());
		return targetDefinition ? findFirstMetaData(*targetDefinition, metaData, definitionManager) : nullptr;
	}

	//--------------------------------------------------------------------------
	ObjectHandle findFirstMetaData(
		const IClassDefinition& targetDefinition, const MetaData & metaData,
		const IDefinitionManager & definitionManager) override
	{
		auto meta = &metaData;
		while (*meta != nullptr)
		{
			auto root = reflectedRoot(meta->getHandle(), definitionManager);
			auto rootDefinition = definitionManager.getDefinition(root.type().getName());
			TF_ASSERT(rootDefinition != nullptr);
			if (rootDefinition->canBeCastTo(targetDefinition))
			{
				return meta->getHandle();
			}
			meta = &meta->next();
		}
		return nullptr;
	}

	//--------------------------------------------------------------------------
	ObjectHandle MetaUtilities::findFirstMetaData(
		const TypeId& typeId, const PropertyAccessor& accessor,
		const IDefinitionManager & definitionManager) override
	{
		auto && metaData = accessor.getMetaData();
		auto targetMetaData = findFirstMetaData(typeId, metaData, definitionManager);
		if (targetMetaData != nullptr)
		{
			return targetMetaData;
		}

		return nullptr;
	}


	//--------------------------------------------------------------------------
	ObjectHandle findFirstMetaData(
		const TypeId& typeId, const IBaseProperty& pProperty,
		const IDefinitionManager & definitionManager) override
	{
		auto && metaData = pProperty.getMetaData();
		return findFirstMetaData(typeId, metaData, definitionManager);
	}

	//--------------------------------------------------------------------------
	ObjectHandle findFirstMetaData(
		const TypeId& typeId, const IClassDefinition& definition,
		const IDefinitionManager & definitionManager) override
	{
		auto && metaData = definition.getMetaData();
		return findFirstMetaData(typeId, metaData, definitionManager);
	}

	//------------------------------------------------------------------------------
	void forEachMetaData(
		const TypeId& typeId, const MetaData & metaData,
		const IDefinitionManager & definitionManager, MetaDataCallback callback) override
	{
		auto targetDefinition = definitionManager.getDefinition(typeId.getName());
		if (targetDefinition == nullptr)
		{
			return;
		}
		forEachMetaData(*targetDefinition, metaData, definitionManager, callback);
	}

	//------------------------------------------------------------------------------
	void forEachMetaData(
		const IClassDefinition & targetDefinition, const MetaData & metaData,
		const IDefinitionManager & definitionManager, MetaDataCallback callback) override
	{
		auto meta = &metaData;
		while (*meta != nullptr)
		{
			auto root = reflectedRoot(meta->getHandle(), definitionManager);
			auto rootDefinition = definitionManager.getDefinition(root.type().getName());
			TF_ASSERT(rootDefinition != nullptr);
			if (rootDefinition->canBeCastTo(targetDefinition))
			{
				callback(meta->getHandle());
			}
			meta = &meta->next();
		}
	}

	//------------------------------------------------------------------------------
	MetaData&& add(MetaData&& left, MetaData&& right) override
	{
		if (left == nullptr)
		{
			return std::move(right);
		}
		setNextMetaData(left, std::move(right));
		return std::move(left);
	}

	//--------------------------------------------------------------------------
	void add(MetaData& left, MetaData&& right) override
	{
		if (left == nullptr)
		{
			left = std::move(right);
		}
		else
		{
			setNextMetaData(left, std::move(right));
		}
	}

	//--------------------------------------------------------------------------
	void setNextMetaData(MetaData& left, MetaData&& right)
	{
		// traverse to the end of the linked list
		auto && next = &left.next();
		const MetaData * last = &left;
		while (*next != nullptr)
		{
			last = next;
			next = &next->next();
		};

		// hook into the end
		last->setNext(std::move(right));
	}
};

} // end namespace wgt
#endif // META_UTILITIES_HPP