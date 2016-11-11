#ifndef RESIZING_MEMORY_STREAM_HPP
#define RESIZING_MEMORY_STREAM_HPP

#include "i_datastream.hpp"
#include "serialization_dll.hpp"
#include <string>

namespace wgt
{
class SERIALIZATION_DLL ResizingMemoryStream : public IDataStream
{
public:
	typedef std::string Buffer;

	ResizingMemoryStream();
	explicit ResizingMemoryStream(Buffer buffer);
	ResizingMemoryStream(ResizingMemoryStream&& that);

	ResizingMemoryStream& operator=(ResizingMemoryStream&& that);

	const Buffer& buffer() const;
	Buffer takeBuffer();
	void setBuffer(Buffer buffer);
	void clear();

	std::streamoff seek(std::streamoff offset, std::ios_base::seekdir dir = std::ios_base::beg) override;
	std::streamsize read(void* destination, std::streamsize size) override;
	std::streamsize write(const void* source, std::streamsize size) override;
	bool sync() override;

private:
	Buffer buffer_;
	std::streamoff pos_;
};

} // end namespace wgt
#endif // RESIZING_MEMORY_STREAM_HPP
