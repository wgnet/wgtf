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
    , length_( 0 )
    , externallyOwned_( false )
{
    copy( data, len, externallyOwned );
}

//------------------------------------------------------------------------------
BinaryBlock::BinaryBlock( const BinaryBlock& b )
    : data_( nullptr )
    , length_( 0 )
    , externallyOwned_( false )
{
    copy( b.data_, b.length_, b.externallyOwned_ );
}

//------------------------------------------------------------------------------
BinaryBlock& BinaryBlock::operator=( const BinaryBlock& b )
{
    if( b != *this )
    {
        copy( b.data_, b.length_, b.externallyOwned_ );
    }
    return *this;
}

//------------------------------------------------------------------------------
void BinaryBlock::copy( const void * data, std::size_t len, bool externallyOwned )
{
    externallyOwned_ = externallyOwned;
    length_ = len;

    if (!externallyOwned)
    {
        data_ = new char[ len ];
        if(data)
        {
            memcpy( data_, data, len );
        }
    }
    else
    {
        data_ = const_cast<void*>( data );
    }
}

//------------------------------------------------------------------------------
BinaryBlock::~BinaryBlock()
{
    if (externallyOwned_)
    {
        return;
    }
    delete[] ( char * ) data_;
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
