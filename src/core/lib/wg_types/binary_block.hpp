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
	BinaryBlock( const BinaryBlock& that );
	BinaryBlock( BinaryBlock&& that );
	~BinaryBlock();

	BinaryBlock& operator=( const BinaryBlock& that );
	BinaryBlock& operator=( BinaryBlock&& that );

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
	void *	data_;
	std::size_t	length_;
	bool    externallyOwned_;
};
} // end namespace wgt
#endif //BINARY_BLOCK_HPP
