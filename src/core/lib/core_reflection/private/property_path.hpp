#ifndef PROPERTY_PATH_IMPL_HPP
#define PROPERTY_PATH_IMPL_HPP

#include "../interfaces/i_property_path.hpp"
#include "core_variant/variant.hpp"
#include "wg_types/shared_string.hpp"
#include "../reflection_dll.hpp"
#include <memory>

namespace wgt
{

class REFLECTION_DLL BasePropertyPath
	: public IPropertyPath
{
protected:
	BasePropertyPath(
		IPropertyPath::ConstPtr & parent, const char * path = nullptr);

	void postConstruct();
public:
	bool operator==(const char * name) const override;
	bool operator==(const std::string & name) const override;

	IPropertyPath::ConstPtr & getParent() const override;

	const std::string & getPath() const override;
	const SharedString & getRecursivePath() const override;
	uint64_t getHash() const override;
	uint64_t getRecursiveHash() const override;
	IPropertyPath::ConstPtr generateChildPath(IPropertyPath::ConstPtr & self, Variant & key) const override;

protected:
	IPropertyPath::ConstPtr parent_;
	SharedString	path_;
	SharedString	recursivePath_;
	uint64_t		hash_;
	uint64_t		recursiveHash_;
};

class REFLECTION_DLL PropertyPath
	: public BasePropertyPath
{
public:
	PropertyPath(IPropertyPath::ConstPtr & parent, const char * path );
	Type getType() const override;

protected:
	std::string generateDecoratedPath() const override;
};


class REFLECTION_DLL CollectionPath
	: public PropertyPath
{
public:
	CollectionPath(IPropertyPath::ConstPtr & parent, const char * path);
	IPropertyPath::ConstPtr generateChildPath(IPropertyPath::ConstPtr & self, Variant & key) const override;
	Type getType() const override;
};


class CollectionChildPath
	: public BasePropertyPath
{
public:
	CollectionChildPath(IPropertyPath::ConstPtr & parent,  Variant & key );
	std::string generateDecoratedPath() const override;
	Type getType() const override;
};

}

#endif //PROPERTY_PATH_IMPL_HPP