#include "reflected_enum_model_new.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/abstract_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_string_utils/string_utils.hpp"

#include <codecvt>
#include <iterator>

namespace wgt
{
ITEMROLE(display);
ITEMROLE(value);
ITEMROLE(valueType);

namespace
{
static const std::string s_RolesArr[] = {
	ItemRole::valueName, ItemRole::valueTypeName,
};
static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);

class ReflectedEnumItem : public AbstractListItem
{
public:
	ReflectedEnumItem(int index, const std::string& text) : index_(index), text_(text)
	{
	}

	Variant getData(int column, ItemRole::Id roleId) const override
	{
		if (roleId == ItemRole::displayId)
		{
			return text_;
		}
		else if (roleId == ItemRole::valueId)
		{
			return index_;
		}
		else if (roleId == ItemRole::valueTypeId)
		{
			return TypeId::getType<int>().getName();
		}
		return Variant();
	}

	bool setData(int column, ItemRole::Id roleId, const Variant& data) override
	{
		return false;
	}

private:
	int index_;
	std::string text_;
};
} // namespace

ReflectedEnumModelNew::ReflectedEnumModelNew(const PropertyAccessor& pA, ObjectHandleT<MetaEnumObj> enumObj)
{
	const wchar_t* enumString = enumObj->getEnumString();
	if (enumString != nullptr)
	{
		int index = 0;
		const wchar_t* start = enumString;
		const wchar_t* enumStringEnd = start + wcslen(start);
		while (start < enumStringEnd)
		{
			const wchar_t* end = nullptr;
			end = wcsstr(start, L"|");
			if (end == nullptr)
			{
				end = start + wcslen(start);
			}
			const wchar_t* trueEnd = end;
			const wchar_t* indexStart = wcsstr(start, L"=");
			if (indexStart != nullptr && indexStart <= end)
			{
				index = static_cast<int>(wcstol(indexStart + 1, nullptr, 10));
				end = indexStart;
			}
			std::wstring text(start, end);

			items_.push_back(new ReflectedEnumItem(index, StringUtils::to_string(text)));
			start = trueEnd + 1;
			++index;
		}
		return;
	}

	Collection collection = enumObj->generateEnum(pA.getObject());
	auto it = collection.begin();
	auto itEnd = collection.end();
	for (; it != itEnd; ++it)
	{
		int index;
		it.key().tryCast(index);
		Variant itValue = it.value();
		std::string text;
		itValue.tryCast(text);
		items_.push_back(new ReflectedEnumItem(index, text));
	}
}

ReflectedEnumModelNew::~ReflectedEnumModelNew()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

//------------------------------------------------------------------------------
void ReflectedEnumModelNew::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	for (auto&& role : s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
std::vector<std::string> ReflectedEnumModelNew::roles() const
{
	return s_RolesVec;
}

//------------------------------------------------------------------------------
AbstractItem* ReflectedEnumModelNew::item(int row) const /* override */
{
	TF_ASSERT(row >= 0);
	const auto index = static_cast<std::vector<AbstractItem*>::size_type>(row);
	TF_ASSERT(index < items_.size());
	return items_[index];
}

int ReflectedEnumModelNew::index(const AbstractItem* item) const /* override */
{
	auto it = std::find(items_.begin(), items_.end(), item);
	TF_ASSERT(it != items_.end());
	return static_cast<int>(std::distance(items_.begin(), it));
}

int ReflectedEnumModelNew::rowCount() const /* override */
{
	return static_cast<int>(items_.size());
}

int ReflectedEnumModelNew::columnCount() const /* override */
{
	return 1;
}
} // end namespace wgt
