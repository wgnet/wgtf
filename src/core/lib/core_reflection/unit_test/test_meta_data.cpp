#include "pch.hpp"

#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/metadata/meta_base.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_object/managed_object.hpp"
#include "test_helpers.hpp"
#include "test_objects.hpp"

namespace wgt
{
TEST_F(TestDefinitionFixture, test_add_meta_data)
{
    auto& manager = getDefinitionManager();
	auto meta = MetaData(nullptr);
	meta += MetaReadOnly() + MetaDirectInvoke();
    meta += MetaHidden();

    CHECK(findFirstMetaData<MetaReadOnlyObj>(meta, manager) != nullptr);
    CHECK(findFirstMetaData<MetaDirectInvokeObj>(meta, manager) != nullptr);
    CHECK(findFirstMetaData<MetaHiddenObj>(meta, manager) != nullptr);
}

TEST_F(TestDefinitionFixture, test_definition_meta_data)
{
    auto object = ManagedObject<TestMetaDataObject>::make();
    auto handle = object.getHandleT();
    auto& definition = *getDefinitionManager().getDefinition<TestMetaDataObject>();

    testMetaData<TestMetaDataObject>(definition, [&](const TestMetaDataObject* data)
    {
        CHECK(data == nullptr);
    });

    testMetaData<MetaReadOnlyObj>(definition, [&](const MetaReadOnlyObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->isReadOnly(handle));
    });

    testMetaData<MetaHiddenObj>(definition, [&](const MetaHiddenObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->isHidden(handle));
    });
}

TEST_F(TestDefinitionFixture, test_member_no_meta_data)
{
    auto object = ManagedObject<TestMetaDataObject>::make();
    auto handle = object.getHandleT();
    auto& definition = *getDefinitionManager().getDefinition<TestMetaDataObject>();
    PropertyAccessor paNone = definition.bindProperty("noMetaData", handle);
    CHECK(paNone.isValid());
    CHECK(paNone.getProperty()->getMetaData() == nullptr);

    testMetaData<MetaReadOnlyObj>(paNone, [&](const MetaReadOnlyObj* data)
    {
        CHECK(data == nullptr);
    });
}

TEST_F(TestDefinitionFixture, test_member_meta_data)
{
    auto object = ManagedObject<TestMetaDataObject>::make();
    auto handle = object.getHandleT();
    auto& definition = *getDefinitionManager().getDefinition<TestMetaDataObject>();
    PropertyAccessor pa = definition.bindProperty("hasMetaData", handle);
    CHECK(pa.isValid());
    CHECK(pa.getProperty()->getMetaData() != nullptr);

    testMetaData<TestMetaDataObject>(pa, [&](const TestMetaDataObject* data)
    {
        CHECK(data == nullptr);
    });

    testMetaData<MetaReadOnlyObj>(pa, [&](const MetaReadOnlyObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->isReadOnly(handle));
    });

    testMetaData<MetaGroupObj>(pa, [&](const MetaGroupObj* data)
    {
        CHECK(data != nullptr);
        CHECK(wcscmp(data->getGroupName(handle), L"group") == 0);
    });

    testMetaData<MetaDecimalsObj>(pa, [&](const MetaDecimalsObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->getDecimals() == 1);
    });

    testMetaData<MetaMinMaxObj>(pa, [&](const MetaMinMaxObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->getMax() == 100);
        CHECK(data->getMin() == -100);
    });

    testMetaData<MetaDescriptionObj>(pa, [&](const MetaDescriptionObj* data)
    {
        CHECK(data != nullptr);
        CHECK(wcscmp(data->getDescription(), L"description") == 0);
    });

    testMetaData<MetaStepSizeObj>(pa, [&](const MetaStepSizeObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->getStepSize() == 0.0001f);
    });

    testMetaData<MetaEnumObj>(pa, [&](const MetaEnumObj* data)
    {
        CHECK(data != nullptr);
        CHECK(wcscmp(data->getEnumString(), L"A=0|B|C") == 0);
    });

    testMetaData<MetaSliderObj>(pa, [&](const MetaSliderObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaNoNullObj>(pa, [&](const MetaNoNullObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaColorObj>(pa, [&](const MetaColorObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaHDRColorObj>(pa, [&](const MetaHDRColorObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaHiddenObj>(pa, [&](const MetaHiddenObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaThumbnailObj>(pa, [&](const MetaThumbnailObj* data)
    {
        CHECK(data != nullptr);
        CHECK(data->getHeight() == 100);
        CHECK(data->getWidth() == 100);
    });

    testMetaData<MetaInPlaceObj>(pa, [&](const MetaInPlaceObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaSelectedObj>(pa, [&](const MetaSelectedObj* data)
    {
        CHECK(data != nullptr);
        CHECK(strcmp(data->getPropName(), "selected") == 0);
    });

    testMetaData<MetaNoSerializationObj>(pa, [&](const MetaNoSerializationObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaDirectInvokeObj>(pa, [&](const MetaDirectInvokeObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaTimeObj>(pa, [&](const MetaTimeObj* data)
    {
        CHECK(data != nullptr);
    });

    testMetaData<MetaComponentObj>(pa, [&](const MetaComponentObj* data)
    {
        CHECK(data != nullptr);
        CHECK(strcmp(data->getComponentName(), "component") == 0);
    });

    testMetaData<MetaAttributeDisplayNameObj>(pa, [&](const MetaAttributeDisplayNameObj* data)
    {
        CHECK(data != nullptr);
        CHECK(strcmp(data->getAttributeName(), "name") == 0);
    });

    testMetaData<MetaDisplayNameObj>(pa, [&](const MetaDisplayNameObj* data)
    {
        CHECK(data != nullptr);
        CHECK(wcscmp(data->getDisplayName(), L"name") == 0);
    });
}

} // end namespace wgt
