#ifndef BINARY_BLOCK_HPP
#define BINARY_BLOCK_HPP

#include <cstddef>

namespace wgt
{
class BinaryBlock
{
public:
    BinaryBlock( const void * data, std::size_t len, bool externallyOwned );
	~BinaryBlock();

	const void * data() const;
	char *	cdata() const;
	std::size_t length() const;
	int compare( BinaryBlock& that ) const;

private:
	void *	data_;
	std::size_t	length_;
	bool    externallyOwned_;
};
} // end namespace wgt
#endif //BINARY_BLOCK_HPP
