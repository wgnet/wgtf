#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/variant.hpp"

#include <cassert>


namespace wgt
{
struct ReflectedMethodParameters::Implementation
{
	Implementation( ReflectedMethodParameters& self ): self_( self ) {}

	ReflectedMethodParameters& self_;
	std::vector<Variant> parameters_;
};


ReflectedMethodParameters::ReflectedMethodParameters()
	: impl_( new Implementation( *this ) )
{
}


ReflectedMethodParameters::ReflectedMethodParameters( const Variant& variant )
	: impl_( new Implementation( *this ) )
{
	impl_->parameters_.push_back( variant );
}


ReflectedMethodParameters::ReflectedMethodParameters( const ReflectedMethodParameters& rhs )
	: impl_( new Implementation( *this ) )
{
	impl_->parameters_ = rhs.impl_->parameters_;
}


ReflectedMethodParameters::~ReflectedMethodParameters()
{
	delete impl_;
}


ReflectedMethodParameters& ReflectedMethodParameters::operator=( const ReflectedMethodParameters& rhs )
{
	if (this != &rhs)
	{
		impl_->parameters_ = rhs.impl_->parameters_;
	}

	return *this;
}


ReflectedMethodParameters& ReflectedMethodParameters::operator,( const Variant& variant )
{
	impl_->parameters_.push_back( variant );
	return *this;
}


bool ReflectedMethodParameters::empty() const
{
	return impl_->parameters_.empty();
}


size_t ReflectedMethodParameters::size() const
{
	return impl_->parameters_.size();
}


void ReflectedMethodParameters::clear()
{
	impl_->parameters_.clear();
}


void ReflectedMethodParameters::push_back( const Variant& variant )
{
	impl_->parameters_.push_back( variant );
}


ReflectedMethodParameters::reference ReflectedMethodParameters::operator[]( size_t index )
{
	return impl_->parameters_[index];
}


ReflectedMethodParameters::const_reference ReflectedMethodParameters::operator[]( size_t index ) const
{
	return impl_->parameters_[index];
}


ReflectedMethodParameters::const_iterator::const_iterator( const ReflectedMethodParameters& collection, size_t index )
	: collection_( const_cast<ReflectedMethodParameters&>( collection ) )
	, index_( index )
{
	assert( index_ <= collection_.size() );
}


ReflectedMethodParameters::const_reference ReflectedMethodParameters::const_iterator::operator*() const
{
	return collection_[index_];
}


ReflectedMethodParameters::const_pointer ReflectedMethodParameters::const_iterator::operator->() const
{
	return &operator*();
}


ReflectedMethodParameters::const_iterator ReflectedMethodParameters::const_iterator::operator++( int )
{
	assert( index_ < collection_.size() );
	return const_iterator( collection_, index_ + 1 );
}


ReflectedMethodParameters::const_iterator& ReflectedMethodParameters::const_iterator::operator++()
{
	assert( index_ < collection_.size() );
	++index_;
	return *this;
}


ReflectedMethodParameters::const_iterator
ReflectedMethodParameters::const_iterator::operator+( difference_type n ) const
{
	return const_iterator( collection_, index_ + size_t( n ) );
}


ReflectedMethodParameters::const_iterator::difference_type
ReflectedMethodParameters::const_iterator::operator-( const const_iterator& rhs ) const
{
	assert( &collection_ == &rhs.collection_ );
	return index_ - rhs.index_;
}


bool ReflectedMethodParameters::const_iterator::operator==( const const_iterator& rhs ) const
{
	return index_ == rhs.index_ && &collection_ == &rhs.collection_;
}


bool ReflectedMethodParameters::const_iterator::operator!=( const const_iterator& rhs ) const
{
	return index_ != rhs.index_ || &collection_ != &rhs.collection_;
}


bool ReflectedMethodParameters::const_iterator::operator<( const const_iterator& rhs ) const
{
	return &collection_ < &rhs.collection_ || (&collection_ == &rhs.collection_ && index_ < rhs.index_);
}


ReflectedMethodParameters::iterator::iterator( ReflectedMethodParameters& collection, size_t index )
	: const_iterator( collection, index )
{
}


ReflectedMethodParameters::reference ReflectedMethodParameters::iterator::operator*() const
{
	return collection_[index_];
}


ReflectedMethodParameters::pointer ReflectedMethodParameters::iterator::operator->() const
{
	return &operator*();
}


ReflectedMethodParameters::iterator ReflectedMethodParameters::iterator::operator++( int )
{
	assert( index_ < collection_.size() );
	return iterator( collection_, index_ + 1 );
}


ReflectedMethodParameters::iterator& ReflectedMethodParameters::iterator::operator++()
{
	assert( index_ < collection_.size() );
	++index_;
	return *this;
}


ReflectedMethodParameters::iterator ReflectedMethodParameters::iterator::operator+( difference_type n ) const
{
	return iterator( collection_, index_ + size_t( n ) );
}


ReflectedMethodParameters::const_iterator ReflectedMethodParameters::cbegin() const
{
	return ReflectedMethodParameters::const_iterator( *this, 0 );
}


ReflectedMethodParameters::const_iterator ReflectedMethodParameters::cend() const
{
	return ReflectedMethodParameters::const_iterator( *this, size() );
}


ReflectedMethodParameters::iterator ReflectedMethodParameters::begin()
{
	return ReflectedMethodParameters::iterator( *this, 0 );
}


ReflectedMethodParameters::iterator ReflectedMethodParameters::end()
{
	return ReflectedMethodParameters::iterator( *this, size() );
}
} // end namespace wgt
