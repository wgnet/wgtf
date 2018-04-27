#ifndef I_OBJECT_HANDLE_STORAGE_HPP
#define I_OBJECT_HANDLE_STORAGE_HPP

#include "core_variant/type_id.hpp"
#include "core_reflection/reflection_dll.hpp"
#include "core_common/signal.hpp"
#include <memory>

#define ENFORCE_OBJECT_HANDLE_CONTEXT assert(DependsBase::hasContext());

namespace wgt
{
class IObjectHandleProvider;
class RefObjectId;
typedef std::shared_ptr<class IObjectHandleStorage> ObjectHandleStoragePtr;

class REFLECTION_DLL IObjectHandleStorage
{
public:
    virtual ~IObjectHandleStorage() = default;

	virtual bool isValid() const = 0;
    virtual IObjectHandleProvider* provider() const = 0;
	virtual void* data() const = 0;
	virtual TypeId type() const = 0;
    virtual const RefObjectId& id() const = 0;
	virtual void setParent(const ObjectHandleStoragePtr& parent, const std::string& path) = 0;
	virtual ObjectHandleStoragePtr parent() const = 0;
	virtual std::string path() const = 0;

	typedef void StorageDestroyed();
	typedef std::function<StorageDestroyed> StorageDestroyedCallback;
	virtual Connection connectDestroyed(StorageDestroyedCallback cb) = 0;

	virtual bool operator==(const IObjectHandleStorage & other) const = 0;
	virtual uint64_t getHashcode() const = 0;
};

} // end namespace wgt

namespace std
{
	template <>
	struct hash<wgt::IObjectHandleStorage> : public unary_function<wgt::IObjectHandleStorage, size_t>
	{
	public:
		size_t operator()(const wgt::IObjectHandleStorage& v) const
		{
			hash<uint64_t> hash_fn;
			return hash_fn(v.getHashcode());
		}
	};

	template <>
	struct hash<const wgt::IObjectHandleStorage> : public unary_function<const wgt::IObjectHandleStorage, size_t>
	{
	public:
		size_t operator()(const wgt::IObjectHandleStorage& v) const
		{
			hash<uint64_t> hash_fn;
			return hash_fn(v.getHashcode());
		}
	};
}

#endif // I_OBJECT_HANDLE_STORAGE_HPP
