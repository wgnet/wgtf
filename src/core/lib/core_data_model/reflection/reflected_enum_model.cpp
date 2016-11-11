#include "reflected_enum_model.hpp"
#include "reflected_item.hpp"

#include "core_data_model/i_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/property_accessor.hpp"

#include "core_string_utils/string_utils.hpp"
#include <codecvt>

namespace wgt
{
namespace
{
class ReflectedEnumItem : public IItem
{
public:
	ReflectedEnumItem(int index, const std::string& text) : index_(index), text_(text)
	{
	}

	const char* getDisplayText(int column) const
	{
		return text_.c_str();
	}

	ThumbnailData getThumbnail(int column) const
	{
		return nullptr;
	}

	Variant getData(int column, ItemRole::Id roleId) const
	{
		if (roleId == ValueRole::roleId_)
		{
			return Variant(index_);
		}
		else if (roleId == IndexPathRole::roleId_)
		{
			return text_ + std::to_string(index_);
		}
		return Variant();
	}

	bool setData(int column, ItemRole::Id roleId, const Variant& data)
	{
		return false;
	}

private:
	int index_;
	std::string text_;
};

void generateFromString(std::vector<IItem*>& items, const wchar_t* enumString)
{
	std::wstring_convert<Utf16to8Facet> conversion(Utf16to8Facet::create());
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

		items.push_back(new ReflectedEnumItem(index, conversion.to_bytes(text)));
		start = trueEnd + 1;
		++index;
	}
}
}

ReflectedEnumModel::ReflectedEnumModel(const MetaEnumObj* enumObj)
{
	const wchar_t* enumString = enumObj->getEnumString();
	if (enumString != nullptr)
	{
		generateFromString(items_, enumString);
	}
}

ReflectedEnumModel::ReflectedEnumModel(const PropertyAccessor& pA, const MetaEnumObj* enumObj)
{
	const wchar_t* enumString = enumObj->getEnumString();
	if (enumString != nullptr)
	{
		generateFromString(items_, enumString);
		return;
	}

	auto value = pA.getParent().getValue();
	ObjectHandle baseProvider;
	value.tryCast(baseProvider);
	Collection collection = enumObj->generateEnum(baseProvider, *pA.getDefinitionManager());
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

ReflectedEnumModel::~ReflectedEnumModel()
{
	for (auto it = items_.begin(); it != items_.end(); ++it)
	{
		delete *it;
	}
}

IItem* ReflectedEnumModel::item(size_t index) const
{
	assert(index < items_.size());
	return items_[index];
}

size_t ReflectedEnumModel::index(const IItem* item) const
{
	auto it = std::find(items_.begin(), items_.end(), item);
	assert(it != items_.end());
	return it - items_.begin();
}

bool ReflectedEnumModel::empty() const
{
	return items_.empty();
}

size_t ReflectedEnumModel::size() const
{
	return items_.size();
}

int ReflectedEnumModel::columnCount() const
{
	return 1;
}
} // end namespace wgt
