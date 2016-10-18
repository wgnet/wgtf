#include "text_stream.hpp"
#include "i_datastream.hpp"
#include <cstdint>
#include <cctype> // for isspace

namespace wgt
{
void TextStream::skipWhiteSpace()
{
	while (true)
	{
		int c = get();
		if (isspace(c) == 0)
		{
			unget();
			break;
		}
	}
}

bool TextStream::beginReadField()
{
	if (!good())
	{
		return false;
	}

	skipWhiteSpace();

	return good();
}

void TextStream::serializeString(IDataStream& dataStream)
{
	(*this) << '"';
	while (true)
	{
		char c;
		if (dataStream.read(&c, 1) <= 0)
		{
			break;
		}

		switch (c)
		{
		case '\\':
			(*this) << "\\\\";
			break;

		case '"':
			(*this) << "\\\"";
			break;

		case '\0':
			(*this) << "\\0";
			break;

		case '\r':
			(*this) << "\\r";
			break;

		case '\n':
			(*this) << "\\n";
			break;

		default:
			(*this) << c;
			break;
		}
	}
	*this << '"';
}

void TextStream::deserializeString(IDataStream& dataStream)
{
	if (!beginReadField())
	{
		return;
	}

	if (get() != '"')
	{
		// string must begin from quote
		setState(std::ios_base::failbit);
		return;
	}

	bool escape = false;
	for (int c = get(); c != EOF; c = get())
	{
		char t = static_cast<char>(c);
		if (!escape)
		{
			switch (t)
			{
			case '\\':
				escape = true;
				continue;

			case '"':
				// got closing quote, we're done
				return;
			}
		}
		else
		{
			escape = false;
			switch (t)
			{
			case '\\':
				break;

			case '"':
				break;

			case '0':
				t = '\0';
				break;

			case 'r':
				t = '\r';
				break;

			case 'n':
				t = '\n';
				break;

			default:
				// unexpected escape char
				setState(std::ios_base::failbit);
				return;
			}
		}

		dataStream.write(&t, 1);
	}

	// unexpected EOF
	setState(std::ios_base::failbit);
}

TextStream& operator<<(TextStream& stream, void* value)
{
	DataStreamBuf buf(stream);
	std::ostream std_stream(&buf);

	std_stream << "0x";

	std_stream.setf(std::ios_base::hex, std::ios_base::basefield);
	std_stream.setf(std::ios_base::right, std::ios_base::adjustfield);

	std_stream.width(sizeof(value) * 2);
	std_stream.fill('0');

	std_stream << reinterpret_cast<uintptr_t>(value);

	stream.setState(std_stream.rdstate());

	return stream;
}

TextStream& operator>>(TextStream& stream, void*& value)
{
	if (!stream.beginReadField())
	{
		return stream;
	}

	DataStreamBuf buf(stream);
	std::istream std_stream(&buf);

	std_stream.unsetf(std::ios_base::basefield); // detect base prefix

	uintptr_t tmp = 0;
	std_stream >> tmp;
	value = reinterpret_cast<void*>(tmp);

	stream.setState(std_stream.rdstate());

	return stream;
}
} // end namespace wgt
