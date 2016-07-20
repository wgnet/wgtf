#ifndef BINARY_BLOCK_HPP
#define BINARY_BLOCK_HPP

#include <cstddef>

namespace wgt
{
class BinaryBlock
{
public:
    BinaryBlock();
    BinaryBlock( const void * data, std::size_t len, bool externallyOwned );
    BinaryBlock( const BinaryBlock& b );
    BinaryBlock& operator=( const BinaryBlock& b );
    ~BinaryBlock();

    const void * data() const;
    char *	cdata() const;
    std::size_t length() const;
    int compare( const BinaryBlock& that ) const;

    bool operator==( const BinaryBlock& that ) const
    {
        return compare( that ) == 0;
    }

    bool operator!=( const BinaryBlock& that ) const
    {
        return !( *this == that );
    }

private:

    void copy( const void * data, std::size_t len, bool externallyOwned );

    void *	data_;
    std::size_t	length_;
    bool    externallyOwned_;
};
} // end namespace wgt
#endif //BINARY_BLOCK_HPP
