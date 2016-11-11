#ifndef FIXED_MEMORY_STREAM_HPP
#define FIXED_MEMORY_STREAM_HPP

#include "i_datastream.hpp"
#include "serialization_dll.hpp"

namespace wgt
{
class SERIALIZATION_DLL FixedMemoryStream : public IDataStream
{
public:
	FixedMemoryStream(void* buffer, std::streamsize size);
	FixedMemoryStream(const void* buffer, std::streamsize size);
	explicit FixedMemoryStream(const char* buffer);

	std::streamoff seek(std::streamoff offset, std::ios_base::seekdir dir = std::ios_base::beg) override;
	std::streamsize read(void* destination, std::streamsize size) override;
	std::streamsize write(const void* source, std::streamsize size) override;
	bool sync() override;

private:
	char* buffer_;
	bool readOnly_;
	std::streamoff pos_;
	std::streamsize size_;
};
} // end namespace wgt
#endif // FIXED_MEMORY_STREAM_HPP
