#include "custom_xml_writer.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/interfaces/i_base_property.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "wg_types/binary_block.hpp"
#include "custom_xml_data.hpp"

namespace wgt
{
CustomXmlDataWriter::CustomXmlDataWriter( TextStream& stream )
	:stream_( stream )
	, indent_( 0 )
	, tagOpening_( false )
	, hasChildElements_( false )
{
}


bool CustomXmlDataWriter::write( const Variant& value )
{
	beginOpenTag( value.type()->name() );
	writeValue( value);
	closeTag( value.type()->name() );

	return !fail();
}


void CustomXmlDataWriter::writeValue( const Variant& value )
{
	CustomXmlData data;
	bool isOk = value.tryCast( data );
	assert( isOk );
	if( !isOk )
	{
		stream_.setState( std::ios_base::failbit );
		return;
	}
	beginOpenTag( "name" );
	endOpenTag();
	stream_ << quoted( data.name_ );
	closeTag( "name" );

	beginOpenTag( "filename" );
	endOpenTag();
	stream_ << quoted( data.filename_ );
	closeTag( "filename" );

	beginOpenTag( "createdBy" );
	endOpenTag();
	stream_ << quoted( data.createdBy_ );
	closeTag( "createdBy" );

	beginOpenTag( "visibility" );
	endOpenTag();
	stream_ << data.visibility_;
	closeTag( "visibility" );

	beginOpenTag( "position" );
	endOpenTag();
	stream_ << data.position_;
	closeTag( "position" );

}



void CustomXmlDataWriter::writeIndent()
{
	for( unsigned i = 0; i < indent_; i++ )
	{
		stream_ << "\t";
	}
}


void CustomXmlDataWriter::writeNewline()
{
	stream_ << "\n";
}


void CustomXmlDataWriter::beginOpenTag( const char* tag )
{
	if( tagOpening_ )
	{
		stream_ << ">";
		writeNewline();
	}

	writeIndent();
	stream_ << "<" << tag;
	tagOpening_ = true;
	hasChildElements_ = false;
	indent_ += 1;
}


void CustomXmlDataWriter::endOpenTag()
{
	if( !tagOpening_ )
	{
		return;
	}

	stream_ << ">";
	tagOpening_ = false;
}


void CustomXmlDataWriter::closeTag( const char* tag )
{
	indent_ -= 1;

	if( tagOpening_ )
	{
		stream_ << "/>";
		tagOpening_ = false;
	}
	else
	{
		if( hasChildElements_ )
		{
			writeIndent();
		}
		stream_ << "</" << tag << ">";
	}

	writeNewline();

	hasChildElements_ = true;
}
} // end namespace wgt
