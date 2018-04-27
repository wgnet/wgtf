#ifndef MANAGED_OBJECT_BASE_HPP
#define MANAGED_OBJECT_BASE_HPP

#include "core_object/i_managed_object.hpp"
#include "core_reflection/ref_object_id.hpp"
#include <memory>

namespace wgt
{

class REFLECTION_DLL ManagedObjectBase
	: public IManagedObject
{
public:
	bool set(std::shared_ptr<RootObjectReference>&& reference, std::shared_ptr<ObjectStorage>&& storage) override;

	virtual ObjectHandle getHandle() const override;
    virtual Variant getVariant() const override;

protected:
	ManagedObjectBase(const std::nullptr_t& = nullptr);
	ManagedObjectBase(const ObjectHandleStoragePtr& storage, const RefObjectId& id = RefObjectId::zero());
	ManagedObjectBase(ManagedObjectBase && rhs);
	~ManagedObjectBase();
	ManagedObjectBase & operator=(ManagedObjectBase && rhs);
	ManagedObjectBase & operator=(const std::nullptr_t&);
	bool operator!=(const std::nullptr_t&) const;
	bool operator==(const std::nullptr_t&) const;
	bool operator==(const ManagedObjectBase &) const;

	ManagedObjectBase(const ManagedObjectBase & rhs) = delete;
	ManagedObjectBase & operator=(const ManagedObjectBase & rhs) = delete;

	void * getStoragePointer() const;
	const IDefinitionManager * getDefinitionManager() const override;

	std::shared_ptr<RootObjectReference> reference_;
	std::shared_ptr<ObjectStorage> storage_;
};

} // end namespace wgt

#endif //MANAGED_OBJECT_BASE_HPP
