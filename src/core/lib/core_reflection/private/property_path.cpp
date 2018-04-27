#include "property_path.hpp"
#include "core_variant/collection.hpp"
#include "wg_types/hash_utilities.hpp"

namespace wgt
{

BasePropertyPath::BasePropertyPath(
	IPropertyPath::ConstPtr & parent, const char * path )
	: parent_(parent)
	, path_( path ? path : "" )
	, hash_( 0 )
	, recursiveHash_( parent ? parent->getRecursiveHash() : 0 )
	//Do deep copy
	, recursivePath_( parent ? parent->getRecursivePath().str() : "" )
{
}

void BasePropertyPath::postConstruct()
{
	auto path = generateDecoratedPath();
	hash_ = HashUtilities::compute(path);
	HashUtilities::directCombine(recursiveHash_, hash_);
	recursivePath_ += path;
}

IPropertyPath::ConstPtr & BasePropertyPath::getParent() const
{
	return parent_;
}

uint64_t BasePropertyPath::getHash() const
{
	return hash_;
}

uint64_t BasePropertyPath::getRecursiveHash() const
{
	return recursiveHash_;
}


const std::string & BasePropertyPath::getPath() const
{
	return path_.str();
}

const SharedString & BasePropertyPath::getRecursivePath() const
{
	return recursivePath_;
}

IPropertyPath::ConstPtr BasePropertyPath::generateChildPath(IPropertyPath::ConstPtr & self, Variant & key) const
{
	TF_ASSERT(self.get() == this);
	IPropertyPath::ConstPtr ptr(new CollectionChildPath(self, key));
	return ptr;
}

bool BasePropertyPath::operator==(const char * name) const
{
	return recursivePath_.str() == name;
}

bool BasePropertyPath::operator==(const std::string & name) const
{
	return recursivePath_.str() == name;
}

PropertyPath::PropertyPath(
	IPropertyPath::ConstPtr & parent, const char * path)
	: BasePropertyPath(parent, path )
{
	postConstruct();
}

std::string PropertyPath::generateDecoratedPath() const
{
	if (parent_ == nullptr)
	{
		return path_.str();
	}

	return "." + path_.str();
}


IPropertyPath::Type PropertyPath::getType() const
{
	return IPropertyPath::TYPE_PROPERTY;
}

CollectionPath::CollectionPath(
	IPropertyPath::ConstPtr & parent, const char * path)
	: PropertyPath( parent, path )
{
}


IPropertyPath::ConstPtr CollectionPath::generateChildPath(IPropertyPath::ConstPtr & self, Variant & key) const
{
	TF_ASSERT(self.get() == this);
	IPropertyPath::ConstPtr ptr( new CollectionChildPath(self, key));
	return ptr;
}


IPropertyPath::Type CollectionPath::getType() const
{
	return IPropertyPath::TYPE_COLLECTION;
}

CollectionChildPath::CollectionChildPath(
	IPropertyPath::ConstPtr & parent, Variant & key)
	: BasePropertyPath( parent )
{
	size_t indexKey = 0;
	if (key.tryCast(indexKey))
	{
		path_ += std::to_string(indexKey);
	}
	else
	{
		key.tryCast(path_);
	}
	postConstruct();
}


std::string CollectionChildPath::generateDecoratedPath() const
{
	return Collection::getIndexOpen() + path_.str() + Collection::getIndexClose();
}

IPropertyPath::Type CollectionChildPath::getType() const
{
	return IPropertyPath::TYPE_COLLECTION_ITEM;
}

}

