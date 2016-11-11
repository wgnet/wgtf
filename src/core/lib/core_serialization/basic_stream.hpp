#ifndef BASIC_STREAM_HPP_INCLUDED
#define BASIC_STREAM_HPP_INCLUDED

#include <ios>

//#include "i_datastream.hpp" // break cyclic include: i_datastream.hpp -> variant.hpp -> text_stream.hpp ->
// basic_stream.hpp -> i_datastream.hpp
#include "serialization_dll.hpp"

namespace wgt
{
class IDataStream;

/**
Basic IDataStream wrapper for more convenient IO.
*/
class SERIALIZATION_DLL BasicStream
{
public:
	/**
	Construct basic stream.
	*/
	explicit BasicStream(IDataStream& dataStream);

	/**
	Destructor.
	*/
	~BasicStream();

	/**
	Get underlying data stream used for actual IO.
	*/
	IDataStream& dataStream() const
	{
		return dataStream_;
	}

	std::ios_base::iostate state() const
	{
		return state_;
	}

	/**
	Set state bits.

	New state is bitwise OR of old state and argument: `newState = oldState | state`
	*/
	void setState(std::ios_base::iostate state);

	/**
	Reset state bits.

	New state is bitwise AND of old state and argument complement: `newState = oldState & ~state`
	*/
	void resetState(std::ios_base::iostate state = ~std::ios_base::iostate(0));

	/**
	Check whether stream state is consistent.
	*/
	bool good() const
	{
		return state_ == std::ios_base::goodbit;
	}

	/**
	Check whether stream reading reached EOF.
	*/
	bool eof() const
	{
		return (state_ & std::ios_base::eofbit) != 0;
	}

	/**
	Check whether underlying IO was failed.
	*/
	bool bad() const
	{
		return (state_ & std::ios_base::badbit) != 0;
	}

	/**
	Check whether data formatting or underlying IO was failed.
	*/
	bool fail() const
	{
		return (state_ & (std::ios_base::badbit | std::ios_base::failbit)) != 0;
	}

	/**
	Wrapper for IDataStream::seek().
	*/
	std::streamoff seek(std::streamoff offset, std::ios_base::seekdir dir = std::ios_base::beg);

	/**
	Sync stream.

	Flush output buffer, reset input buffer.
	*/
	bool sync();

	/**
	Current unget buffer size.
	*/
	std::streamsize ungetBufferSize() const
	{
		return ungetBufferSize_;
	}

	/**
	Set unget buffer size.

	Unget buffer is an area in input buffer that is kept actual as far as
	possible. The size of this buffer defines maximum amount of data that
	could be unget().

	Default value is 1, i.e. you can always unget at least one byte, provided
	at least one byte was read right before calling unget().

	@return actual value to be used.

	@see unget
	*/
	std::streamsize setUngetBufferSize(std::streamsize v);

	/**
	Read single character.
	*/
	bool get(char& c);

	/**
	Read single character.

	@return character read or EOF if no character could be retrieved.
	*/
	int get();

	/**
	Undo last read of @a size chars.

	If optional argument @a source is set then function succeeds only if source
	bytes match buffered bytes. If @ source is set to @c nullptr then only
	@c size is considered.

	@return @c true on success, @c false on failure (unget area is not
	large enough or @a source doesn't match buffer)
	*/
	bool unget(std::streamsize size = 1, const void* source = nullptr);

	/**
	Get the next char to be read, but don't advance read position.

	@return character or EOF if no character could be retrieved.
	*/
	int peek();

	/**
	Wrapper for IDataStream::read().
	*/
	std::streamsize read(void* destination, std::streamsize size);

	/**
	Write single character.
	*/
	bool put(char c);

	/**
	Wrapper for IDataStream::write().
	*/
	std::streamsize write(const void* source, std::streamsize size);

	/**
	Read stream until destination buffer is filled or error occurred.

	Arguments and return value meaning are the same as in read(), except that
	negative value is never returned.

	Regular read() function may read less than requested, so use this function
	if you want to read as much as possible.
	*/
	std::streamsize readHard(void* destination, std::streamsize size);

	/**
	Write stream until source buffer is sent or error occurred.

	Arguments and return value meaning are the same as in write(), except that
	negative value is never returned.

	Regular write() function may write less than requested, so use this function
	if you want to write as much as possible.
	*/
	std::streamsize writeHard(const void* source, std::streamsize size);

	/**
	Copy data from @a source stream to this one.

	Copying proceeds until @a amount bytes are copied or some error occurred.
	If @a amount is negative (default) then all data available in @a source is
	copied.

	@return actual amount of copied data.

	@see copyTo
	*/
	std::streamsize copyFrom(IDataStream& source, std::streamsize amount = -1);

	/**
	Copy data from this stream to @a destination.

	Copying proceeds until @a amount bytes are copied or some error occurred.
	If @a amount is negative (default) then all data available in this stream is
	copied.

	@return actual amount of copied data.

	@see copyFrom
	*/
	std::streamsize copyTo(IDataStream& destination, std::streamsize amount = -1);

private:
	// don't allow copying
	BasicStream(const BasicStream&);
	BasicStream& operator=(const BasicStream&);

	IDataStream& dataStream_;

	std::ios_base::iostate state_;

	char readBuffer_[16];
	char* readPos_; // is allowed to get below (readBuffer_ + ungetBufferSize_) only in unget()
	char* readEnd_;
	std::streamsize ungetBufferSize_;

	bool resetReadBuffer();
};
} // end namespace wgt
#endif // BASIC_STREAM_HPP_INCLUDED
