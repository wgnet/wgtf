#ifndef I_PROPERTY_PATH_HPP
#define I_PROPERTY_PATH_HPP

#include <string>
#include <memory>
#include "../reflection_dll.hpp"

namespace wgt
{

class Variant;
class SharedString;

class REFLECTION_DLL IPropertyPath
{
public:
	typedef const std::shared_ptr< const IPropertyPath > ConstPtr;

	enum Type
	{
		TYPE_PROPERTY,
		TYPE_COLLECTION,
		TYPE_COLLECTION_ITEM
	};
	virtual ~IPropertyPath() {}

	virtual std::string generateDecoratedPath() const = 0;
	virtual ConstPtr generateChildPath( IPropertyPath::ConstPtr & self, Variant & key) const = 0;
	virtual ConstPtr & getParent() const = 0;
	virtual Type getType() const = 0;

	virtual uint64_t getHash() const = 0;
	virtual uint64_t getRecursiveHash() const = 0;

	virtual const std::string & getPath() const = 0;
	virtual const SharedString & getRecursivePath() const = 0;
	virtual bool operator==(const char * name) const = 0;
	virtual bool operator==(const std::string & name) const = 0;
};


}

#endif //I_PROPERTY_PATH_HPP