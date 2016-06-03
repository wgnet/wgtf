#include "reflected_object_item.hpp"
#include "reflected_group_item.hpp"
#include "reflected_property_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/generic_tree_model.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_string_utils/string_utils.hpp"

#include <codecvt>

namespace wgt
{
bool CompareWStrings(const wchar_t * a, const wchar_t * b)
{
	return wcscmp(a, b) < 0;
}

ReflectedObjectItem::ReflectedObjectItem( const ObjectHandle & object, ReflectedItem * parent )
	: ReflectedItem( parent, parent ? parent->getPath() + "." : "" )
	, object_( object )
	, groups_(CompareWStrings)
{
}

const IClassDefinition * ReflectedObjectItem::getDefinition() const 
{ 
	return object_.getDefinition( *getDefinitionManager() );
}


const char * ReflectedObjectItem::getDisplayText( int column ) const
{
	switch (column)
	{
	case 0:
		if (displayName_.empty())
		{
			auto definition = getDefinition();
			if (definition == nullptr)
			{
				return nullptr;
			}
			const MetaDisplayNameObj * displayName =
				findFirstMetaData< MetaDisplayNameObj >( *definition, *getDefinitionManager() );
			if (displayName == nullptr)
			{
				displayName_ = definition->getName();
			}
			else
			{
				std::wstring_convert< Utf16to8Facet > conversion( Utf16to8Facet::create() );
				displayName_ = conversion.to_bytes( displayName->getDisplayName() );
			}
		}
		return displayName_.c_str();

	case 1:
		{
			auto definition = getDefinition();
			if (definition == nullptr)
			{
				return "";
			}
			return definition->getName();
		}

	default:
		assert( false );
		return "";
	}
}


Variant ReflectedObjectItem::getData( int column, size_t roleId ) const
{
	// Only works for root items?
	assert( parent_ == nullptr );
	if (roleId == ValueRole::roleId_)
	{
		return object_;
	}

	if (roleId == IndexPathRole::roleId_)
	{
		return this->getPath();
	}
    else if (roleId == ObjectRole::roleId_)
    {
        return getObject();;
    }
    else if (roleId == RootObjectRole::roleId_)
    {
        return getRootObject();
    }

	return Variant();
}

bool ReflectedObjectItem::setData(int column, size_t roleId, const Variant & data)
{
	if(roleId == ValueRole::roleId_)
	{
		ObjectHandle other;
		if(!data.tryCast(other))
			return false;

		auto definitionManager = getDefinitionManager();
		if(definitionManager == nullptr)
			return false;

		auto obj = getRootObject();
		auto definition = obj.getDefinition(*definitionManager);
		auto otherDef = other.getDefinition(*definitionManager);
		if(definition != otherDef)
			return false;

		for(auto prop : definition->allProperties())
		{
			auto accessor = definition->bindProperty(prop->getName(), obj);
			auto otherAccessor = definition->bindProperty(prop->getName(), other);
			if(accessor.canSetValue())
			{
				assert(otherAccessor.canGetValue());
				accessor.setValue(otherAccessor.getValue());
			}
		}
		if(getParent())
		{
			return getParent()->setData(column, roleId, obj);
		}
		return true;
	}
	return false;
}


GenericTreeItem * ReflectedObjectItem::getChild( size_t index ) const
{
	if(children_.size() > index)
		return children_[index].get();

	size_t currentIndex = 0;
	EnumerateChildren([&currentIndex, index](ReflectedItem& item){
		return (currentIndex++ == index);
	});

	if(currentIndex > 0)
		return children_[--currentIndex].get();
	return nullptr;
}

bool ReflectedObjectItem::empty() const
{
	bool isEmpty = true;
	EnumerateChildren([&isEmpty](ReflectedItem&){
		isEmpty = false;
		return false;
	});
	return isEmpty;
}

size_t ReflectedObjectItem::size() const
{
	size_t count = 0;

	EnumerateChildren([&count](ReflectedItem&){
		++count;
		return true;
	});

	return count;
}

//==============================================================================
bool ReflectedObjectItem::preSetValue(
	const PropertyAccessor & accessor, const Variant & value )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->preSetValue( accessor, value ))
		{
			return true;
		}
	}
	return false;
}


//==============================================================================
bool ReflectedObjectItem::postSetValue(
	const PropertyAccessor & accessor, const Variant & value )
{
	for (auto it = children_.begin(); it != children_.end(); ++it)
	{
		if ((*it) == nullptr)
		{
			continue;
		}

		if ((*it)->postSetValue( accessor, value ))
		{
			return true;
		}
	}
	return false;
}

void ReflectedObjectItem::EnumerateChildren(const ReflectedItemCallback& callback) const
{
	// Get the groups and iterate them first
	const auto& groups = GetGroups();

	// This will iterate all the groups and any cached children
	for(auto& child : children_)
	{
		if(!callback(*child.get()))
			return;
	}

	// ReflectedGroupItem children handle grouped items
	int skipChildCount = static_cast<int>(children_.size() - groups.size());
	auto parent = const_cast<ReflectedObjectItem*>(this);
	EnumerateVisibleProperties([&](IBasePropertyPtr property, const std::string & inplacePath)
	{
		bool isGrouped = findFirstMetaData< MetaGroupObj >(*property, *getDefinitionManager()) != nullptr;
		if ( !isGrouped )
		{
			// Skip already iterated children
			if ( --skipChildCount < 0 )
			{
				children_.emplace_back(new ReflectedPropertyItem(property, parent, inplacePath));
				return callback(*children_.back().get());
			}
		}
		return true;
	});
	return;
}

ReflectedObjectItem::Groups& ReflectedObjectItem::GetGroups() const
{
	auto definition = getDefinition();
	if ( !groups_.empty() || definition == nullptr)
	{
		return groups_;
	}

	auto parent = const_cast<ReflectedObjectItem *>( this );
	EnumerateVisibleProperties([this, parent](IBasePropertyPtr property, const std::string & inplacePath){
		const MetaGroupObj * groupObj = findFirstMetaData< MetaGroupObj >(*property, *getDefinitionManager());
		if ( groupObj != nullptr && groups_.insert(groupObj->getGroupName()).second )
		{
			children_.emplace_back( new ReflectedGroupItem( groupObj, parent, inplacePath ) );
		}
		return true;
	});
	return groups_;
}
} // end namespace wgt
