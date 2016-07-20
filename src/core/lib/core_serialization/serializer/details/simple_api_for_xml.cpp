#include "simple_api_for_xml.hpp"
#include <string>
#include <expat.h>


namespace wgt
{
class SimpleApiForXml::Impl
{
public:
	explicit Impl( SimpleApiForXml* sax, TextStream& stream );
	~Impl();

	bool parse();
	void abortParsing();

	bool aborted() const
	{
		return aborted_;
	}

private:
	XML_Parser parser_;
	TextStream& stream_;
	bool aborted_;
	std::streamoff bytesRead_;

};


////////////////////////////////////////////////////////////////////////////////


SimpleApiForXml::Impl::Impl( SimpleApiForXml* sax, TextStream& stream ):
	parser_( XML_ParserCreate( "UTF-8" ) ),
	stream_( stream ),
	aborted_( false ),
	bytesRead_( 0 )
{
	struct Callbacks
	{
		static void XMLCALL StartElementHandler(
			void* userData,
			const XML_Char* name,
			const XML_Char** atts)
		{
			if( !castUserData( userData )->aborted() )
			{
				castUserData( userData )->elementStart( name, atts );
			}
		}

		static void XMLCALL EndElementHandler(
			void* userData,
			const XML_Char* name)
		{
			if( !castUserData( userData )->aborted() )
			{
				castUserData( userData )->elementEnd( name );
			}
		}

		static void XMLCALL CharacterDataHandler(
			void* userData,
			const XML_Char* s,
			int len)
		{
			if( !castUserData( userData )->aborted() )
			{
				castUserData( userData )->characterData( s, len );
			}
		}

		static SimpleApiForXml* castUserData( void* userData )
		{
			return static_cast<SimpleApiForXml*>( userData );
		}
	};

	XML_SetUserData( parser_, sax );

	XML_SetElementHandler(
		parser_,
		&Callbacks::StartElementHandler,
		&Callbacks::EndElementHandler );

	XML_SetCharacterDataHandler(
		parser_,
		&Callbacks::CharacterDataHandler );
}

SimpleApiForXml::Impl::~Impl()
{
	XML_ParserFree( parser_ );
}

bool SimpleApiForXml::Impl::parse()
{
	if( aborted_ )
	{
		return false;
	}

	bool result = false;

	const int BUFFER_SIZE = 1024;
	while( true )
	{
		void* buff = XML_GetBuffer( parser_, BUFFER_SIZE );
		if( buff == NULL )
		{
			// out of memory
			break;
		}

		auto bytes_read = stream_.read( buff, BUFFER_SIZE );
		if( bytes_read < 0 )
		{
			bytes_read = 0;
		}

		bytesRead_ += bytes_read;

		auto r = XML_ParseBuffer(
			parser_,
			static_cast<int>( bytes_read ),
			bytes_read == 0 );

		if( r != XML_STATUS_OK )
		{
			// parsing failed
			break;
		}

		if( bytes_read == 0 )
		{
			result = true;
			break;
		}
	}

	// rewind stream
	std::streamoff off =
		XML_GetCurrentByteIndex( parser_ ) +
		XML_GetCurrentByteCount( parser_ ) -
		bytesRead_;
	if( off != 0 )
	{
		stream_.seek( off, std::ios_base::cur );
		stream_.resetState( std::ios_base::eofbit );
	}

	return result;
}

void SimpleApiForXml::Impl::abortParsing()
{
	if( !aborted_ )
	{
		XML_StopParser( parser_, XML_FALSE );
		aborted_ = true;
	}
}

////////////////////////////////////////////////////////////////////////////////


SimpleApiForXml::SimpleApiForXml( TextStream& stream ):
	impl_( new Impl( this, stream ) )
{
}


SimpleApiForXml::~SimpleApiForXml()
{
	// nop
}


bool SimpleApiForXml::parse()
{
	return impl_->parse();
}


void SimpleApiForXml::abortParsing()
{
	impl_->abortParsing();
}


bool SimpleApiForXml::aborted() const
{
	return impl_->aborted();
}


void SimpleApiForXml::elementStart( const char* name, const char* const* attributes )
{
	// nop
}


void SimpleApiForXml::elementEnd( const char* name )
{
	// nop
}


void SimpleApiForXml::characterData( const char* data, size_t length )
{
	// nop
}
} // end namespace wgt
