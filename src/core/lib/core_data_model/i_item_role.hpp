#ifndef I_ITEM_ROLE_HPP
#define I_ITEM_ROLE_HPP

#include "wg_types/hash_utilities.hpp"

#define ITEMROLE(ROLE)                                   \
	namespace ItemRole                                   \
	{                                                    \
	namespace                                            \
	{                                                    \
	const char* ROLE##Name = #ROLE;                      \
	const Id ROLE##Id = ::wgt::ItemRole::compute(#ROLE); \
	}                                                    \
	}

namespace wgt
{
namespace ItemRole
{
// Wrap the ItemRole's hash into a type that allows use of a visualizer for debugging purposes
struct Id
{
	Id() : hash_(0)
	{
	}

	Id(uint64_t hash) : hash_(hash)
	{
	}

	uint64_t hash_;

	operator uint64_t()
	{
		return hash_;
	}

	bool operator==(const ItemRole::Id& rhs) const
	{
		return hash_ == rhs.hash_;
	}

	bool operator!=(const ItemRole::Id& rhs) const
	{
		return hash_ != rhs.hash_;
	}

	bool operator<(const ItemRole::Id& rhs) const
	{
		return hash_ < rhs.hash_;
	}

	bool operator>(const ItemRole::Id& rhs) const
	{
		return hash_ > rhs.hash_;
	}

	template <typename T>
	inline bool operator==(T rhs) const
	{
		return hash_ == rhs;
	}

	template <typename T>
	inline bool operator!=(T rhs) const
	{
		return hash_ != rhs;
	}

	template <typename T>
	inline bool operator<(T rhs) const
	{
		return hash_ < rhs;
	}

	template <typename T>
	inline bool operator>(T rhs) const
	{
		return hash_ > rhs;
	}
};

template <typename T>
inline bool operator==(T rhs, const ItemRole::Id& lhs)
{
	return rhs == lhs.hash_;
}

template <typename T>
inline bool operator!=(T rhs, const ItemRole::Id& lhs)
{
	return rhs != lhs.hash_;
}

template <typename T>
inline bool operator<(T rhs, const ItemRole::Id& lhs)
{
	return rhs < lhs.hash_;
}

template <typename T>
inline bool operator>(T rhs, const ItemRole::Id& lhs)
{
	return rhs > lhs.hash_;
}

inline ItemRole::Id compute(const char* roleName)
{
	return HashUtilities::compute(roleName);
}
}

// DEPRECATED
// Add new role types here
#define ITEM_ROLES                                              \
	/* Tree Adapter */                                          \
	X(ChildModel, childModel)                                   \
	X(ColumnModel, columnModel)                                 \
	X(HasChildren, hasChildren)                                 \
	X(Description, description)                                 \
	X(Expanded, expanded)                                       \
	X(ParentIndex, parentIndex)                                 \
	X(Selected, selected)                                       \
	X(IndexPath, indexPath)                                     \
	/* Component Extension */                                   \
	X(Component, component)                                     \
	X(ValueType, valueType)                                     \
	X(KeyType, keyType)                                         \
	X(IsAction, isAction)                                       \
	X(IsEnum, isEnum)                                           \
	X(IsThumbnail, isThumbnail)                                 \
	X(IsSlider, isSlider)                                       \
	X(IsColor, isColor)                                         \
	X(IsUrl, isUrl)                                             \
	/* Value Extension */                                       \
	X(Value, value)                                             \
	X(Key, key)                                                 \
	X(MinValue, minValue)                                       \
	X(MaxValue, maxValue)                                       \
	X(StepSize, stepSize)                                       \
	X(Decimals, decimals)                                       \
	X(EnumModel, enumModel)                                     \
	X(Definition, definition)                                   \
	X(Name, name)                                               \
	X(Object, object)                                           \
	X(RootObject, rootObject)                                   \
	X(DefinitionModel, definitionModel)                         \
	X(UrlIsAssetBrowser, urlIsAssetBrowser)                     \
	X(UrlDialogTitle, urlDialogTitle)                           \
	X(UrlDialogDefaultFolder, urlDialogDefaultFolder)           \
	X(UrlDialogNameFilters, urlDialogNameFilters)               \
	X(UrlDialogSelectedNameFilter, urlDialogSelectedNameFilter) \
	/* Asset Item Extension */                                  \
	X(StatusIcon, statusIcon)                                   \
	X(TypeIcon, typeIcon)                                       \
	X(Size, size)                                               \
	X(CreatedTime, createdTime)                                 \
	X(ModifiedTime, modifiedTime)                               \
	X(AccessedTime, accessedTime)                               \
	X(IsDirectory, isDirectory)                                 \
	X(IsReadOnly, isReadOnly)                                   \
	X(IsCompressed, isCompressed)                               \
	/* Thumbnail Extension */                                   \
	X(Thumbnail, thumbnail)                                     \
	X(headerText, headerText)                                   \
	X(footerText, footerText)                                   \
	/*Buttons Extensions*/                                      \
	X(buttonsDefinition, buttonsDefinition)                     \
	X(buttonIcon, buttonIcon)                                   \
	X(buttonEnabled, buttonEnabled)                             \
	X(buttonClicked, buttonClicked)

#define X(ROLE, ROLENAME)                   \
	class ROLE##Role                        \
	{                                       \
	public:                                 \
		static const char* roleName_;       \
		static ::wgt::ItemRole::Id roleId_; \
	};
ITEM_ROLES
#undef X
} // end namespace wgt

namespace std
{
template <>
struct hash<wgt::ItemRole::Id>
{
	typedef wgt::ItemRole::Id argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& s) const
	{
		return static_cast<size_t>(s.hash_);
	}
};
}
#endif // I_ITEM_ROLE_HPP
