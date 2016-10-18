#ifndef I_PROPERTY_MODEL_EXTENSION_HPP
#define I_PROPERTY_MODEL_EXTENSION_HPP

#include "core_variant/variant.hpp"
#include "core_reflection/base_property.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
class ICommandManager;
class IDefinitionManager;
class RefPropertyItem;

struct PropertyNode
{
    enum PropertyType: int32_t
    {
        Invalid = -1,
        SelfRoot = 0,
        RealProperty,
        CollectionItem,
        GroupProperty,
        VirtualProperty, // reserve some range for generic types. I don't know what types it will be now,
                             // but reserve some values is good idea in my opinion
                             DomainSpecificProperty = 255
                             // use can use values DomainSpecificProperty, DomainSpecificProperty + 1, ... , DomainSpecificProperty + n
                             // on you own purpose. It's only way to transfer some information between iterations
    };

    PropertyNode();

    bool operator== (const PropertyNode& other) const;
    bool operator!= (const PropertyNode& other) const;
    bool operator< (const PropertyNode& other) const;

    int32_t propertyType; // it can be value from PropertyType or any value that you set in your extension
    IBasePropertyPtr propertyInstance;
    ObjectHandle object; // this object should be always object that can be transfer into IBasePropertyPtr::setValue() to set new value.
};

class IChildAllocator
{
public:
    virtual ~IChildAllocator() {}

    virtual std::shared_ptr<const PropertyNode> createPropertyNode(IBasePropertyPtr propertyInstance, ObjectHandle object, int32_t type = PropertyNode::RealProperty) = 0;
    virtual IBasePropertyPtr getCollectionItemProperty(std::string&& name, const TypeId& type, IDefinitionManager & defMng) = 0;
};

std::shared_ptr<const PropertyNode> MakeRootNode(ObjectHandle handle, IChildAllocator& allocator);

class ExtensionChain
{
public:
    ExtensionChain(const TypeId& type)
        : typeId(type)
    {
    }

    virtual ~ExtensionChain()
    {
        nextExtension.reset();
    }

    static std::shared_ptr<ExtensionChain> addExtension(std::shared_ptr<ExtensionChain> head, const std::shared_ptr<ExtensionChain>& extension)
    {
        if (extension->typeId != head->typeId)
        {
            assert(false);
            return head;
        }

        extension->nextExtension = head;
        assert(extension->nextExtension != nullptr);
        return extension;
    }

    static std::shared_ptr<ExtensionChain> removeExtension(std::shared_ptr<ExtensionChain> head, const std::shared_ptr<ExtensionChain>& extension)
    {
        if (extension->typeId != head->typeId)
        {
            assert(false);
            return head;
        }

        if (head == extension)
        {
            std::shared_ptr<ExtensionChain> result = extension->nextExtension;
            extension->nextExtension.reset();
            return result;
        }

        head->nextExtension = removeExtension(head->nextExtension, extension);
        return head;
    }

    const TypeId& getType() const
    {
        return typeId;
    }

protected:
    template<typename T>
    T* getNext()
    {
        assert(nextExtension != nullptr);
        assert(dynamic_cast<T*>(nextExtension.get()));
        return static_cast<T*>(nextExtension.get());
    }

    template<typename T>
    const T* getNext() const
    {
        assert(nextExtension != nullptr);
        assert(dynamic_cast<const T*>(nextExtension.get()));
        return static_cast<const T*>(nextExtension.get());
    }

private:
    TypeId typeId;
    std::shared_ptr<ExtensionChain> nextExtension;
};


// The main goal of this extension is create children of some property.
// parent - is property node, that you should create children for.
// children - return value
// use allocator to create children
class ChildCreatorExtension: public ExtensionChain
{
public:
    ChildCreatorExtension();
    virtual void exposeChildren(const std::shared_ptr<const PropertyNode>& parent, std::vector<std::shared_ptr<const PropertyNode>>& children, IDefinitionManager& defMng) const;
    static std::shared_ptr<ChildCreatorExtension> createDummy();

    void setAllocator(std::shared_ptr<IChildAllocator> allocator);

protected:
    std::shared_ptr<IChildAllocator> allocator;
};

// This extension provide way to customize algorithm of set/get data to/from property.
// In getter you can for example calculate value for some virtual property
// And in setter you can make some validation, or update some dependent properties.
class SetterGetterExtension: public ExtensionChain
{
public:
    SetterGetterExtension();
    virtual Variant getValue(const RefPropertyItem* item, int column, ItemRole::Id roleId, IDefinitionManager & definitionManager) const;
    virtual bool setValue(RefPropertyItem * item, int column, ItemRole::Id roleId, const Variant & data,
        IDefinitionManager & definitionManager, ICommandManager & commandManager) const;
    static std::shared_ptr<SetterGetterExtension> createDummy();
};

// This extension should implements custom rules of search ReflectedPropertyItem for some value.
// As ReflectedPropertyItem is real item that user will see, in multi selection case we should make decision in which 
// ReflectedPropertyItem add current property.
// Limitation - node.propertyInstance should be equal of item.property()
// If your extension return nullptr, ReflectedPropertyModel will create new ReflectedPropertyItem for that.
class MergeValuesExtension: public ExtensionChain
{
public:
    MergeValuesExtension();
    virtual RefPropertyItem* lookUpItem(const std::shared_ptr<const PropertyNode>& node, const std::vector<std::unique_ptr<RefPropertyItem>>& items,
        IDefinitionManager & definitionManager) const;
    static std::shared_ptr<MergeValuesExtension> createDummy();
};

// By this extension, you can inject into item some data for some roles. for example information about buttons.
// inject are being called for every new ReflectedPropertyItem.
// updateInjection are being called every time, when MergeValueExtension said as, that we should add/remove PropertyNode
// from ReflectedPropertyItem. You can update state of buttons for example
class InjectDataExtension: public ExtensionChain
{
public:
    InjectDataExtension();
    virtual void inject(RefPropertyItem* item);
    virtual void updateInjection(RefPropertyItem* item);
    static std::shared_ptr<InjectDataExtension> createDummy();
};
} // namespace wgt

#endif