#ifndef PROPERTY_ITERATOR_HPP
#define PROPERTY_ITERATOR_HPP

#include <vector>
#include <memory>

namespace wgt
{
class IClassDefinition;
class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;
class PropertyIteratorImplBase;
typedef std::shared_ptr< PropertyIteratorImplBase > PropertyIteratorImplPtr;

class PropertyIteratorImplBase
{
public:
	virtual ~PropertyIteratorImplBase() {}

	virtual IBasePropertyPtr current() const = 0;
	virtual bool next() = 0;
};

class PropertyIterator
{
public:
	enum IterateStrategy
	{
		ITERATE_PARENTS,
		ITERATE_SELF_ONLY
	};

	PropertyIterator();
	PropertyIterator( IterateStrategy strategy, const IClassDefinition & definition );

	IBasePropertyPtr get() const;

	IBasePropertyPtr operator*() const;
	IBasePropertyPtr operator->() const;

	PropertyIterator & operator++();

	bool operator==(const PropertyIterator& other) const;
	bool operator!=(const PropertyIterator& other) const;

private:
	void moveNext();

	IterateStrategy								strategy_;
	const IClassDefinition *					currentDefinition_;
	PropertyIteratorImplPtr						currentIterator_;
};


// Helper class for use with range-based-for loops and STL algorithms
// Allows std::begin() and std::end() to work for PropertyIterators.
class PropertyIteratorRange
{
public:
	PropertyIteratorRange( PropertyIterator::IterateStrategy strategy, const IClassDefinition & definition );

	PropertyIterator begin() const;
	PropertyIterator end() const;

private:
	PropertyIterator::IterateStrategy strategy_;
	const IClassDefinition & definition_;
};
} // end namespace wgt
#endif // PROPERTY_ITERATOR_HPP
