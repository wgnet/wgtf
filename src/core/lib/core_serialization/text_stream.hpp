#ifndef TEXT_STREAM_HPP_INCLUDED
#define TEXT_STREAM_HPP_INCLUDED

#include "basic_stream.hpp"
#include "datastreambuf.hpp"
#include <iostream>
#include <string>
#include <cstring>

namespace wgt
{
class TextStream:
	public BasicStream
{
	typedef BasicStream base;
public:
	explicit TextStream( IDataStream& dataStream ):
		base( dataStream )
	{
	}

	/**
	Skip white space characters.

	This function uses standard function `isspace()`.
	*/
	void skipWhiteSpace();

	/**
	Prepare to read field.

	This function skips whitespaces (see isspace() standard function) and checks
	whether this stream is good.

	@see BasicStream::good()
	*/
	bool beginReadField();

	/**
	Serialize all data from the @a dataStream as string.

	Use this utility function to serialize arbitrary data as text string.

	@see deserializeString
	*/
	void serializeString( IDataStream& dataStream );

	/**
	Deserialize string to the @a dataStream.
	*/
	void deserializeString( IDataStream& dataStream );
};


template<typename T>
struct TextStreamTraits
	{
private:
	struct Yes {};
	struct No {};

	template<typename U>
	static Yes checkStreamingOut(typename std::remove_reference<decltype(std::declval<std::ostream&>() << std::declval<const U&>())>::type*);
	
	template<typename U>
	static No checkStreamingOut(...);
	
	template<typename U>
	static Yes checkStreamingIn(typename std::remove_reference<decltype(std::declval<std::istream&>() >> std::declval<U&>())>::type*);
	
	template<typename U>
	static No checkStreamingIn(...);
	
public:
	static const bool has_std_streaming_out = std::is_same<decltype(checkStreamingOut<T>(0)), Yes>::value;
	static const bool has_std_streaming_in = std::is_same<decltype(checkStreamingIn<T>(0)), Yes>::value;

};


// Use std::ostream operator<< overload by default
template< typename T >
typename std::enable_if<
	TextStreamTraits< T >::has_std_streaming_out &&
	( !std::is_pointer< T >::value || std::is_convertible< T, const char* >::value ),
TextStream& >::type operator<<( TextStream& stream, const T& v )
{
	DataStreamBuf buf( stream );
	std::ostream std_stream( &buf );
	std_stream << v;
	stream.setState( std_stream.rdstate() );
	return stream;
}


// Use std::istream operator>> overload by default
template<typename T>
typename std::enable_if<
	TextStreamTraits<T>::has_std_streaming_in &&
	!std::is_pointer< T >::value,
TextStream& >::type operator>>( TextStream& stream, T& v )
{
	DataStreamBuf buf( stream );
	std::istream std_stream( &buf );
	std_stream >> v;
	stream.setState( std_stream.rdstate() );
	return stream;
}


// pointer serialization
TextStream& operator<<( TextStream& stream, void* value );
TextStream& operator>>( TextStream& stream, void*& value );

template< typename T >
typename std::enable_if< std::is_pointer< T >::value, TextStream& >::type
	operator>>( TextStream& stream, T& v )
{
	void* tmp = (void*)v;
	stream >> tmp;
	v = ( T )tmp;
	return stream;
}
} // end namespace wgt
#endif // TEXT_STREAM_HPP_INCLUDED
