#include "standard_metatypes.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"


namespace
{

	using namespace wgt;

	bool convertToString( std::string* to, const MetaType* fromType, const void* from )
	{
		ResizingMemoryStream dataStream;
		TextStream stream( dataStream );
		fromType->streamOut( stream, from );
		if( stream.fail() )
		{
			return false;
		}

		stream.sync();
		*to = dataStream.takeBuffer();
		return true;
	}

	template< typename To, typename From >
	bool straightConvert( To* to, const MetaType* fromType, const void* from )
	{
		if( fromType->typeId() != TypeId::getType< From >() )
		{
			return false;
		}

		if( !to || !from )
		{
			return true;
		}

		const From* f = reinterpret_cast< const From* >( from );

		*to = static_cast< To >( *f );

		return true;
	}

}


namespace wgt
{

	MetaTypeImpl< void >::MetaTypeImpl():
		base( "void", DeducibleFromText )
	{
	}


	void MetaTypeImpl< void >::init( void* value ) const
	{
		// nop
	}


	void MetaTypeImpl< void >::copy( void* dest, const void* src ) const
	{
		// nop
	}


	void MetaTypeImpl< void >::move( void* dest, void* src ) const
	{
		// nop
	}


	void MetaTypeImpl< void >::destroy( void* value ) const
	{
		// nop
	}


	bool MetaTypeImpl< void >::equal( const void* lhs, const void* rhs ) const
	{
		return true;
	}


	void MetaTypeImpl< void >::streamOut( TextStream& stream, const void* value ) const
	{
		stream << "void";
	}


	void MetaTypeImpl< void >::streamIn( TextStream& stream, void* value ) const
	{
		stream >> match( "void" );
	}


	void MetaTypeImpl< void >::streamOut( BinaryStream& stream, const void* value ) const
	{
		// nop
	}


	void MetaTypeImpl< void >::streamIn( BinaryStream& stream, void* value ) const
	{
		// nop
	}


	bool MetaTypeImpl< void >::convertFrom( void* to, const MetaType* fromType, const void* from ) const
	{
		return
			base::convertFrom( to, fromType, from ) ||
			true;
	}


	////////////////////////////////////////////////////////////////////////////


	MetaTypeImpl< uintmax_t >::MetaTypeImpl():
		base( "uint", DeducibleFromText )
	{
	}


	bool MetaTypeImpl< uintmax_t >::convertFrom( void* to, const MetaType* fromType, const void* from ) const
	{
		return
			base::convertFrom( to, fromType, from ) ||
			straightConvert< uintmax_t, intmax_t >( &base::cast( to ), fromType, from ) ||
			straightConvert< uintmax_t, double >( &base::cast( to ), fromType, from );
	}


	////////////////////////////////////////////////////////////////////////////


	MetaTypeImpl< intmax_t >::MetaTypeImpl():
		base( "int", DeducibleFromText )
	{
	}


	bool MetaTypeImpl< intmax_t >::convertFrom( void* to, const MetaType* fromType, const void* from ) const
	{
		return
			base::convertFrom( to, fromType, from ) ||
			straightConvert< intmax_t, uintmax_t >( &base::cast( to ), fromType, from ) ||
			straightConvert< intmax_t, double >( &base::cast( to ), fromType, from );
	}


	////////////////////////////////////////////////////////////////////////////


	MetaTypeImpl< double >::MetaTypeImpl():
		base( "real", DeducibleFromText )
	{
	}


	bool MetaTypeImpl< double >::convertFrom( void* to, const MetaType* fromType, const void* from ) const
	{
		return
			base::convertFrom( to, fromType, from ) ||
			straightConvert< double, uintmax_t >( &base::cast( to ), fromType, from ) ||
			straightConvert< double, intmax_t >( &base::cast( to ), fromType, from );
	}


	////////////////////////////////////////////////////////////////////////////


	MetaTypeImpl< std::string >::MetaTypeImpl():
		base( "string", ForceShared | DeducibleFromText )
	{
	}


	void MetaTypeImpl< std::string >::streamOut(TextStream& stream, const void* value) const
	{
		stream << quoted( base::cast(value) );
	}


	void MetaTypeImpl< std::string >::streamIn(TextStream& stream, void* value) const
	{
		stream >> quoted( base::cast(value) );
	}


	void MetaTypeImpl< std::string >::streamOut(BinaryStream& stream, const void* value) const
	{
		stream << base::cast(value);
	}


	void MetaTypeImpl< std::string >::streamIn(BinaryStream& stream, void* value) const
	{
		stream >> base::cast(value);
	}


	bool MetaTypeImpl< std::string >::convertFrom( void* to, const MetaType* fromType, const void* from ) const
	{
		return
			base::convertFrom( to, fromType, from ) ||
			convertToString( &base::cast( to ), fromType, from );
	}

}
