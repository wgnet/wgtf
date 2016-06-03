#ifndef SIMPLE_API_FOR_XML_HPP_INCLUDED
#define SIMPLE_API_FOR_XML_HPP_INCLUDED

#include "core_serialization/text_stream.hpp"
#include <vector>
#include <string>
#include <expat.h>

namespace wgt
{
class SimpleApiForXml
{
public:
	explicit SimpleApiForXml( TextStream& stream );
	~SimpleApiForXml();

	bool parse();

protected:
	// Not just `abort()` to avoid accidental `::abort()` call.
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

	virtual void elementStart( const char* name, const char* const* attributes );
	virtual void elementEnd( const char* name );
	virtual void characterData( const char* data, size_t length );

};
} // end namespace wgt
#endif
