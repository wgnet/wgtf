#include "binary_block.hpp"
#include <cstring>
#include <cassert>
#include <memory>

namespace wgt
{
//------------------------------------------------------------------------------
BinaryBlock::BinaryBlock()
	: data_( nullptr )
	, length_( 0 )
	, externallyOwned_( false )
{
}


//------------------------------------------------------------------------------
BinaryBlock::BinaryBlock( const void * data, size_t len, bool externallyOwned )
	: data_( nullptr )
	, length_( len )
	, externallyOwned_( externallyOwned )
{
	if (externallyOwned == false)
	{
		data_ = new char[ len ];
		memcpy( data_, data, len );
	}
	else
	{
		data_ = const_cast<void*>( data );
	}
}


//------------------------------------------------------------------------------
BinaryBlock::BinaryBlock( const BinaryBlock& that )
	: data_( nullptr )
	, length_( that.length_ )
	, externallyOwned_( that.externallyOwned_ )
{
	if (externallyOwned_)
	{
		data_ = that.data_;
	}
	else
	{
		data_ = new char[ length_ ];
		memcpy( data_, that.data_, length_ );
	}
}


//------------------------------------------------------------------------------
BinaryBlock::BinaryBlock( BinaryBlock&& that )
	: data_( that.data_ )
	, length_( that.length_ )
	, externallyOwned_( that.externallyOwned_ )
{
	if (!that.externallyOwned_)
	{
		that.data_ = nullptr;
		that.length_ = 0;
	}
}


//------------------------------------------------------------------------------
BinaryBlock::~BinaryBlock()
{
	if (!externallyOwned_)
	{
		delete[] ( char * ) data_;
	}
}


//------------------------------------------------------------------------------
BinaryBlock& BinaryBlock::operator=( const BinaryBlock& that )
{
	if (this == &that)
	{
		return *this;
	}

	if (!externallyOwned_)
	{
		delete[] ( char * ) data_;
	}

	length_ = that.length_;
	externallyOwned_ = that.externallyOwned_;

	if (externallyOwned_)
	{
		data_ = that.data_;
	}
	else
	{
		data_ = new char[ length_ ];
		memcpy( data_, that.data_, length_ );
	}

	return *this;
}


//------------------------------------------------------------------------------
BinaryBlock& BinaryBlock::operator=( BinaryBlock&& that )
{
	if (this == &that)
	{
		return *this;
	}

	if (!externallyOwned_)
	{
		delete[] ( char * ) data_;
	}

	data_ = that.data_;
	length_ = that.length_;
	externallyOwned_ = that.externallyOwned_;

	if (!that.externallyOwned_)
	{
		that.data_ = nullptr;
		that.length_ = 0;
	}

	return *this;
}


//------------------------------------------------------------------------------
const void * BinaryBlock::data() const
{
	return data_;
}


//------------------------------------------------------------------------------
char * BinaryBlock::cdata() const
{
	return reinterpret_cast< char * >( data_ );
}


//------------------------------------------------------------------------------
size_t BinaryBlock::length() const
{
	return length_;
}


//------------------------------------------------------------------------------
int BinaryBlock::compare( const BinaryBlock& that ) const
{
	if (!this->data_ && !that.data_)
	{
		return 0;
	}

	if (this->length_ < that.length_)
	{
		return -1;
	}

	if (this->length_ > that.length_)
	{
		return 1;
	}
	assert( this->length_ >= 0 );
	return memcmp( this->data_, that.data_, (size_t)this->length_ );
}
} // end namespace wgt
