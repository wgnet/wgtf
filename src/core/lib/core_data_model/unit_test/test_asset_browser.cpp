#include "pch.hpp"

#include "core_data_model/asset_browser/file_system_asset_presentation_provider.hpp"
#include "core_data_model/asset_browser/i_asset_object_item.hpp"
#include "core_unit_test/unit_test.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_dll.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"

namespace wgt
{
class MockAssetObjectItem : public IAssetObjectItem
{
public:
	MockAssetObjectItem(const std::string& name) : assetName_(name)
	{
	}

	virtual const char* getDisplayText(int column) const override
	{
		return nullptr;
	}

	virtual ThumbnailData getThumbnail(int column) const override
	{
		return nullptr;
	}

	virtual Variant getData(int column, ItemRole::Id roleId) const override
	{
		return nullptr;
	}

	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data) override
	{
		return false;
	}

	virtual const IItem* getParent() const override
	{
		return nullptr;
	}

	virtual IItem* operator[](size_t index) const override
	{
		return nullptr;
	}

	virtual size_t indexOf(const IItem* item) const override
	{
		return 0;
	}

	virtual bool empty() const override
	{
		return false;
	}

	virtual size_t size() const override
	{
		return 0;
	}

	virtual ThumbnailData getStatusIconData() const override
	{
		return nullptr;
	}

	virtual uint16_t getAssetType() const override
	{
		return 0;
	}

	virtual const char* getFullPath() const override
	{
		return nullptr;
	}
	virtual uint64_t getSize() const override
	{
		return 0;
	}

	virtual uint64_t getCreatedTime() const override
	{
		return 0;
	}

	virtual uint64_t getModifiedTime() const override
	{
		return 0;
	}

	virtual uint64_t getAccessedTime() const override
	{
		return 0;
	}

	virtual bool isDirectory() const override
	{
		return false;
	}

	virtual bool isReadOnly() const override
	{
		return true;
	}

	virtual bool isCompressed() const override
	{
		return false;
	}

	virtual const char* getAssetName() const override
	{
		return assetName_.c_str();
	}

private:
	std::string assetName_;
};

TEST(asset_browser_presentation_provider)
{
	wchar_t path[MAX_PATH];
	::GetModuleFileNameW(0, path, MAX_PATH);
	::PathRemoveFileSpecW(path);

	FileSystemAssetPresentationProvider provider;
	provider.generateData(StringUtils::to_string(path) + FilePath::kNativeDirectorySeparator);

	MockAssetObjectItem statusIcon("item");
	CHECK(provider.getStatusIconData(&statusIcon) != nullptr);

	auto testThumbnail = [&](const std::string& extension, bool isNull) {
		MockAssetObjectItem item("item" + extension);
		auto thumbnail = provider.getThumbnail(&item);
		CHECK(isNull ? thumbnail == nullptr : thumbnail != nullptr);
	};

	testThumbnail(".default", false);
	testThumbnail(".txt", false);
	testThumbnail(".dll", false);
	testThumbnail(".pdb", false);
	testThumbnail(".xml", false);
	testThumbnail(".png", false);
	testThumbnail(".bmp", false);
	testThumbnail(".jpg", false);
	testThumbnail("", true);
}
} // end namespace wgt
