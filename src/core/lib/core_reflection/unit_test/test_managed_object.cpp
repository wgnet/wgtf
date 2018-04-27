#include "pch.hpp"

#include "core_object/managed_object.hpp"
#include "core_reflection/utilities/object_handle_reflection_utils.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_variant/collection.hpp"
#include "test_reflection_fixture.hpp"
#include "core_object/object_handle_provider.hpp"

namespace wgt
{
struct ObjectUnreflected
{
};

struct ObjectBase : public ObjectUnreflected
                  , public ObjectHandleProvider<ObjectBase>
                  , Depends<IDefinitionManager>
{
	virtual ~ObjectBase(){}
	int value = 0;
};

struct ObjectDerived : Implements<ObjectBase>
{
    ObjectDerived() { value = 1;  }
    ObjectDerived(int x, std::string s = "") { value = x; }
};

BEGIN_EXPOSE(ObjectBase)
END_EXPOSE()

BEGIN_EXPOSE(ObjectDerived, MetaDirectBases( ObjectBase ) )
END_EXPOSE()

void registerTestObjects(IDefinitionManager& manager)
{
	manager.registerDefinition<TypeClassDefinition<ObjectDerived>>();
	manager.registerDefinition<TypeClassDefinition<ObjectBase>>();
}

TEST(test_make_empty_managed_object)
{
	ManagedObject<ObjectDerived> object = nullptr;
	CHECK(object == nullptr);
	CHECK(object.getHandleT() == nullptr);
	CHECK(object.getHandle() == nullptr);
	CHECK(!object.getHandle().isValid());
}

TEST_F(TestReflectionFixture, test_make_managed_object)
{
	registerTestObjects(getDefinitionManager());

    auto testObject = [&](ManagedObject<ObjectDerived> object)
    {
        CHECK(object != nullptr);
        CHECK(std::is_reference<decltype(*object)>::value);
        CHECK(object->value == 1);
        object->value = 2;

        auto handleUntyped = object.getHandle();
        CHECK(handleUntyped != nullptr);
        CHECK(handleUntyped.isValid());

        auto handle = object.getHandleT();
        auto handleAlt = object.getHandleT();
        CHECK(handle != nullptr);
        CHECK(handleAlt != nullptr);

        auto data = handle.get();
        CHECK(data != nullptr);
        CHECK(data->value == 2);
        data->value = 3;

        auto pointer = object.getPointer();
        CHECK(pointer != nullptr);
        CHECK(pointer->value == 3);
        pointer->value = 4;
        CHECK(object->value == 4);

        auto objMoved = std::move(object);
        CHECK(objMoved != nullptr);
        CHECK(object == nullptr);
        CHECK(handle != nullptr);
        CHECK(handle->value == 4);

        objMoved = nullptr;
        CHECK(objMoved == nullptr);
        CHECK(handle == nullptr);
        CHECK(handleAlt == nullptr);
        CHECK(handleUntyped == nullptr);
        CHECK(!handleUntyped.isValid());
    };

    testObject(ManagedObject<ObjectDerived>::make());
    testObject(ManagedObject<ObjectDerived>::make(1, "hello"));
}

TEST_F(TestReflectionFixture, test_make_untyped_managed_object)
{
	registerTestObjects(getDefinitionManager());

    auto testIObject = [&](std::unique_ptr<IManagedObject> object)
    {
        auto handleUntyped = object->getHandle();
        CHECK(handleUntyped != nullptr);
        CHECK(handleUntyped.isValid());

        auto handleDerived = object->getHandleT<ObjectDerived>();
        CHECK(handleDerived != nullptr);
        CHECK(handleDerived.get() != nullptr);
        CHECK(handleDerived->value == 1);

        auto handleBase = object->getHandleT<ObjectBase>();
        CHECK(handleBase != nullptr);
        CHECK(handleBase.get() != nullptr);
        CHECK(handleBase->value == 1);

        CHECK(handleUntyped == handleDerived);
        CHECK(handleUntyped == ObjectHandle(handleDerived));
        CHECK(handleBase == handleDerived);

        CHECK(handleUntyped == handleBase);
        CHECK(handleUntyped == ObjectHandle(handleBase));
        CHECK(ObjectHandle(handleBase) == ObjectHandle(handleDerived));

        auto handleUnreflected = object->getHandleT<ObjectUnreflected>();
        CHECK(handleUnreflected == nullptr);

        object.reset();
        CHECK(handleUntyped == nullptr);
        CHECK(!handleUntyped.isValid());
        CHECK(handleDerived == nullptr);
        CHECK(handleBase == nullptr);
    };

    auto testObject = [&](std::unique_ptr<ManagedObject<ObjectDerived>> object)
    {
        CHECK(object);
        CHECK(*object != nullptr);
        CHECK((*object)->value == 1);

        auto handle = object.get()->getHandleT();
        CHECK(handle != nullptr);

        ManagedObjectPtr untypedObject(std::move(object));
        CHECK(handle != nullptr);
        testIObject(std::move(untypedObject));
        CHECK(handle == nullptr);
    };

    testObject(ManagedObject<ObjectDerived>::make_unique());
    testObject(ManagedObject<ObjectDerived>::make_unique(1, "hello"));
    testIObject(ManagedObject<ObjectDerived>::make_iunique());
    testIObject(ManagedObject<ObjectDerived>::make_iunique(1, "hello"));
    testIObject(ManagedObject<ObjectDerived>::make_iunique_fn([&](auto&){}));
}

TEST_F(TestReflectionFixture, test_managed_object_collection)
{
    registerTestObjects(getDefinitionManager());

    ObjectHandle handle = nullptr;
    {
        typedef std::vector<ManagedObjectPtr> ContainerType;
        auto collectionHolder = std::make_shared<CollectionHolder<ContainerType>>();

        ContainerType& container = collectionHolder->storage();
        for (int i = 0; i < 5; ++i)
        {
            container.push_back(ManagedObject<ObjectDerived>::make_iunique(i));
        }

        Collection collection(collectionHolder);
        collectionHolder.reset();
        CHECK(collection.isValid());
        CHECK(collection.size() == 5);
        auto ownedContainer = collection.container<ContainerType>();
        CHECK(ownedContainer);

        for (int i = 5; i < 10; ++i)
        {
            ownedContainer->push_back(ManagedObject<ObjectDerived>::make_iunique(i));
        }

        CHECK(collection.isValid());
        CHECK(collection.size() == 10);
        for (int i = 0; i < (int)collection.size(); ++i)
        {
            handle = nullptr;
            CHECK(collection[i].tryCast(handle));
            CHECK(handle != nullptr);
            CHECK(handle.isValid());
            auto obj = handle.getBase<ObjectDerived>();
            CHECK(obj != nullptr);
            CHECK(obj->value == i);
        }
    }
    CHECK(handle == nullptr);
}

TEST_F(TestReflectionFixture, test_managed_object_handle_provider)
{
    registerTestObjects(getDefinitionManager());

    auto object = ManagedObject<ObjectDerived>::make();
    CHECK(object->handle() == object.getHandleT());
    CHECK(object->handle() == object.getHandle());
}

} // end namespace wgt
