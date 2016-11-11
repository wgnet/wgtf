#ifndef I_DATASTREAM_HPP
#define I_DATASTREAM_HPP

#include "serialization_dll.hpp"

// BEGIN DEPRECATED
#include "core_variant/type_id.hpp"
#include "core_variant/variant.hpp"
#include "core_common/deprecated.hpp"
// END DEPRECATED

#include <cassert>
#include <ios>
#include <type_traits>

namespace wgt
{
/**
Base class for data streams.

All streams nominally support both reading and writing. But implementations
of read- or write-only streams can leave just a failing stub in place of write()
or read() respectively.

A stream can optionally support current position which is used for both reading
and writing. In this case it's assumed that both readingg and writing use the same
storage, i.e. you can read what you have just written by seeking back to
the size of written data. If the stream doesn't support seeking then seek() returns
negative value (indicating failure).

@todo Remove all functions marked as `DEPRECATED`, remove default implementations
of `read` and `write` functions (currently they're implemented using deprecated
interface).
*/
class SERIALIZATION_DLL IDataStream
{
public:
	virtual ~IDataStream();

	/**
	Set current read/write position.

	@param offset - offset relative to @a anchor to seek
	@param dir - specifies from whence to count @a offset

	@return resulting position relative to begin, negative value
	if the stream is not seekable or some error occured.
	*/
	virtual std::streamoff seek(std::streamoff offset, std::ios_base::seekdir dir = std::ios_base::beg) = 0;

	/**
	Read data from stream.

	@param destination pointer to a destination buffer
	@param size maximum amount of data to read into the @a destination

	@return actual amount of data read, 0 or less if EOF was reached or other error.

	@note Result can be less than @a size.
	*/
	virtual std::streamsize read(void* destination, std::streamsize size);

	/**
	Write data to stream.

	@param source pointer to a buffer that contains data to write
	@param size maximum amount of data to write from the @a source

	@return actual amount of data written, 0 or less if some error occured.

	@note Result can be less than @a size.
	*/
	virtual std::streamsize write(const void* source, std::streamsize size);

	/**
	Synchronize the data stream with underlying storage backend.

	Call this function to ensure that all written data gets its final storage.
	Read buffer (if any) is also reset, so the next read operation will see
	fresh data.
	*/
	virtual bool sync() = 0;

	// deprecated functions

	DEPRECATED virtual size_t pos() const;
	DEPRECATED virtual size_t size() const;
	DEPRECATED virtual const void* rawBuffer() const;
	DEPRECATED virtual size_t readRaw(void* o_Data, size_t length);
	DEPRECATED virtual size_t writeRaw(const void* data, size_t length);
	DEPRECATED virtual bool eof() const;

	template <typename T>
	DEPRECATED typename std::enable_if<!std::is_pointer<T>::value && Variant::traits<T>::can_downcast, bool>::type read(
	T& t)
	{
		bool br = false;
		Variant variant = t;
		br = read(variant);
		if (br)
		{
			t = variant.cast<T>();
		}
		else
		{
			assert(false);
		}
		return br;
	}

	DEPRECATED bool read(Variant& variant);

	template <typename T>
	DEPRECATED typename std::enable_if<!std::is_pointer<T>::value && Variant::traits<T>::can_upcast, bool>::type write(
	const T& t)
	{
		Variant variant = t;
		return write(variant);
	}

	DEPRECATED bool write(const Variant& variant);

private:
	DEPRECATED virtual bool writeValue(const Variant& variant);
	DEPRECATED virtual bool readValue(Variant& variant);
};
} // end namespace wgt
#endif
