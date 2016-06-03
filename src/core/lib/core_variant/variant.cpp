#include "variant.hpp"
#include "interfaces/i_meta_type_manager.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/std_data_stream.hpp"

#include <stdexcept>

#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdio>
#include <cassert>


#ifndef _DUMMY_
namespace wgt
{
namespace
{

	static IMetaTypeManager * s_metaTypeManager = nullptr;

	bool wtoutf8( const wchar_t * wsrc, std::string& output )
	{
		if (wsrc == nullptr)
		{
			output.clear();
			return true;
		}

		std::wstring_convert< Utf16to8Facet > conversion(
			Utf16to8Facet::create() );

		output = conversion.to_bytes( wsrc );
		return true;
	}

	bool utf8tow( const char * src, std::wstring& output )
	{
		if (src == nullptr)
		{
			output.clear();
			return true;
		}

		std::wstring_convert< Utf16to8Facet > conversion(
			Utf16to8Facet::create() );

		output = conversion.from_bytes( src );
		return true;
	}
}


std::string upcast(const char* v)
{
	return v ? v : "";
}

std::string upcast(const std::wstring& v)
{
	std::string str;
	wtoutf8( v.c_str(), str );
	return str;
}

std::string upcast(const wchar_t* v)
{
	std::string str;
	wtoutf8( v, str );
	return str;;
}

bool downcast(std::wstring* v, const std::string& storage)
{
	if(v)
	{
		utf8tow( storage.c_str(), *v );
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////


Variant::DynamicData* Variant::DynamicData::allocate(size_t payloadSize)
{
	char* data = new char[sizeof(DynamicData) + payloadSize];
	return new (data) DynamicData();
}


void Variant::DynamicData::decRef(const MetaType* type)
{
	if (refs_.fetch_sub( 1 ) == 0)
	{
		assert(type);
		type->destroy(payload());
		this->~DynamicData();
		delete[] reinterpret_cast<char*>(this);
	}
}


////////////////////////////////////////////////////////////////////////////////


Variant::Variant()
{
	initVoid();
}


Variant::Variant(const Variant& value)
{
	init(value);
}


Variant::Variant(Variant&& value)
{
	init(std::move(value));
}


Variant::Variant(const MetaType* type):
	type_(type)
{
	assert(type_);

	void* p;
	if(isInline())
	{
		p = data_.payload_;
	}
	else
	{
		data_.dynamic_ = DynamicData::allocate(type_->size());
		p = data_.dynamic_->payload();
	}

	type_->init(p);
}


Variant::Variant(const MetaType* type, const Variant& value):
	type_(type)
{
	assert(type_);

	void* p;
	if(isInline())
	{
		p = data_.payload_;
	}
	else
	{
		data_.dynamic_ = DynamicData::allocate(type_->size());
		p = data_.dynamic_->payload();
	}

	type_->init(p);

	if( !type_->convertFrom( payload(), value.type_, value.payload() ) )
	{
		typeInitError();
	}
}


Variant& Variant::operator=( const Variant& value )
{
	if( this == &value )
	{
		return *this;
	}

	if( type_ == value.type_ )
	{
		detach( false );
		type_->copy( payload(), value.payload() );
	}
	else
	{
		destroy();
		init( value );
	}

	return *this;
}


Variant& Variant::operator=( Variant&& value )
{
	if( this == &value )
	{
		return *this;
	}

	if( type_ == value.type_ )
	{
		detach( false );
		type_->move( payload(), value.payload() );
	}
	else
	{
		destroy();
		init( std::move( value ) );
	}

	return *this;
}


bool Variant::operator==(const Variant& v) const
{
	if(type_ == v.type_)
	{
		const void* lp = payload();
		const void* rp = v.payload();

		return
			lp == rp ||
			type_->equal(lp, rp);
	}

	Variant tmp(type_);
	if(!type_->convertFrom(tmp.payload(), v.type_, v.payload()))
	{
		return false;
	}

	assert(type_ == tmp.type_);

	const void* lp = payload();
	const void* rp = tmp.payload();

	return
		lp == rp ||
		type_->equal(lp, rp);
}


bool Variant::convert(const MetaType* type)
{
	if(!type)
	{
		return false;
	}

	Variant tmp(type);

	if(!type->convertFrom(tmp.payload(), type_, payload()))
	{
		return false;
	}

	*this = std::move(tmp);

	return true;
}


bool Variant::isVoid() const
{
	return typeIs<void>();
}


bool Variant::isPointer() const
{
	return type_->pointedType() != nullptr;
}


void Variant::initVoid()
{
	type_ = findType<void>();
	if(!type_)
	{
		typeInitError();
	}
}


/**
Initialize variant.
@warning This function assumes uninitialized/undefined state of variant on entry,
so it should be used with care.
@see destroy
*/
void Variant::init(const Variant& value)
{
	type_ = value.type_;
	assert(type_);

	if(isInline())
	{
		type_->init(data_.payload_);
		type_->copy(data_.payload_, value.data_.payload_);
	}
	else
	{
		data_.dynamic_ = value.data_.dynamic_;
		data_.dynamic_->incRef();
	}
}


void Variant::init(Variant&& value)
{
	type_ = value.type_;
	assert(type_);

	if(isInline())
	{
		type_->init(data_.payload_);
		type_->move(data_.payload_, value.data_.payload_);
	}
	else
	{
		data_.dynamic_ = value.data_.dynamic_;
		data_.dynamic_->incRef();
	}
}


/**
Destroy currently held value and free all external resources used by it.
@warning This function leaves variant in uninitialized/undefined state and
breaks invariants, so it should be used with care.
@see init
*/
void Variant::destroy()
{
	if( isInline() )
	{
		type_->destroy( data_.payload_ );
	}
	else
	{
		data_.dynamic_->decRef( type_ );
	}
}


/**
Ensure we hold an exclusive payload instance.

If actual detach happens then @a copy parameter specifies whether old value
should be copied to a new one. Otherwise new value is left default-initialized.
*/
void Variant::detach( bool copy )
{
	if( isInline() || data_.dynamic_->isExclusive() )
	{
		return;
	}

	// allocate and initialize new payload copy
	DynamicData* newDynamic = DynamicData::allocate( type_->size() );
	type_->init( newDynamic->payload() );
	if( copy )
	{
		type_->copy( newDynamic->payload(), data_.dynamic_->payload() );
	}

	// substitute payload with the new one (which is obviously exclusive)
	data_.dynamic_->decRef( type_ );
	data_.dynamic_ = newDynamic;
}


void Variant::castError()
{
#ifdef _WIN32
#if ( _MSC_VER >= 1900 )
	throw std::bad_cast::__construct_from_string_literal("Variant cast failed");
#else
	throw std::bad_cast("Variant cast failed");
#endif
#else
	throw std::bad_cast();
#endif
}


void Variant::typeInitError()
{
#ifdef _WIN32
#if ( _MSC_VER >= 1900 )
	throw std::bad_cast::__construct_from_string_literal("type is not registered in Variant");
#else
	throw std::bad_cast("type is not registered in Variant");
#endif
#else
	throw std::bad_cast();
#endif
}


void Variant::setMetaTypeManager( IMetaTypeManager* metaTypeManager)
{
	s_metaTypeManager = metaTypeManager;
}


IMetaTypeManager * Variant::getMetaTypeManager()
{
	return s_metaTypeManager;
}


bool Variant::registerType(const MetaType* type)
{
	if(s_metaTypeManager)
	{
		return s_metaTypeManager->registerType(type);
	}
	else
	{
		return false;
	}
}


////////////////////////////////////////////////////////////////////////////////


TextStream& operator<<(TextStream& stream, const Variant& value)
{
	value.type()->streamOut(stream, value.payload());
	return stream;
}


TextStream& operator>>(TextStream& stream, Variant& value)
{
	if(!stream.beginReadField())
	{
		return stream;
	}

	if(!value.isVoid())
	{
		value.type()->streamIn(stream, value.payload());
		stream.peek();
		return stream;
	}

	enum
	{
		UnknownType, // used for the first char only
		LeadingZero, // if the first char is zero
		Int, // signed/unsigned integer number
		Double, // integer part of real number
		DoubleFractional, // fractional part of real number
		DoubleExponent // exponent part of real number (e-notation)
	} state = UnknownType;

	int sign = 1;
	bool hadSign = false;

	uint64_t uintValue = 0;
	int64_t intValue = 0;
	int intBase = 10;
	unsigned digits = 0;

	double doubleValue = 0.0;
	double doubleFactor = 1.0;

	while(stream.good())
	{
		const int c = stream.get();
		switch(state)
		{
		case UnknownType:
			switch(c)
			{
			case EOF:
				// unexpected end of stream
				stream.setState(std::ios_base::failbit);
				return stream;

			case '"': // string
				{
					stream.unget();
					auto stringType = Variant::findType<std::string>();
					assert(stringType);
					Variant tmp(stringType);
					stringType->streamIn(stream, tmp.payload());
					if(!stream.fail())
					{
						value = std::move(tmp);
					}
				}
				//calling peek here is trying to set eofbit
				// if next value in stream reaches EOF
				stream.peek();
				return stream;

			case 'v':
				{
					stream.unget();
					auto voidType = Variant::findType<void>();
					assert(voidType);
					Variant tmp(voidType);
					voidType->streamIn(stream, tmp.payload());
					if(!stream.fail())
					{
						value = std::move(tmp);
					}
				}
				//calling peek here is trying to set eofbit
				// if next value in stream reaches EOF
				stream.peek();
				return stream;

			case '.':
			case ',':
				// float
				state = DoubleFractional;
				continue;

			case '0':
				state = LeadingZero;
				continue;

			case '-':
			case '+':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// positive integer
				stream.unget();
				state = Int;
				continue;

			default: // unknown type
				stream.unget();
				stream.setState(std::ios_base::failbit);
				return stream;
			}
			continue;

		case LeadingZero:
			switch(tolower(c))
			{
			case 'x': // hex
				intBase = 16;
				state = Int;
				continue;

			case 'b': // bin
				intBase = 2;
				state = Int;
				continue;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// oct
				stream.unget();
				intBase = 8;
				state = Int;
				continue;

			case '.':
			case ',':
				// double/float
				state = DoubleFractional;
				continue;

			case EOF:
			default:
				if(c != EOF)
				{
					stream.unget();
				}
				value = 0;
				return stream;
			}
			continue;

		case Int:
		case DoubleExponent:
			{
				int l = tolower(c);
				int digit;
				switch(l)
				{
				case '-':
				case '+':
					if(!hadSign && digits == 0)
					{
						hadSign = true;
						if(l == '-')
						{
							sign = -1;
						}
						continue;
					}
					else
					{
						// invalid digit
						digit = intBase;
						break;
					}

				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					digit = l - '0';
					digits += 1;
					break;

				case 'e':
					// scientific notation
					if(intBase == 10)
					{
						if(state == Int)
						{
							if(sign > 0)
							{
								doubleValue = static_cast<double>(uintValue);
							}
							else
							{
								doubleValue = static_cast<double>(intValue);
							}
						}
						else
						{
							// invalid digit
							digit = intBase;
							break;
						}

						state = DoubleExponent;

						// reset int state
						sign = 1;
						hadSign = false;
						uintValue = 0;
						intValue = 0;
						intBase = 10;
						digits = 0;

						continue;
					}
					// no break

				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'f':
					digit = l - 'a' + 10;
					digits += 1;
					break;

				case '.':
				case ',':
					// fractional separator
					if(intBase == 10)
					{
						if(state == Int)
						{
							if(sign > 0)
							{
								doubleValue = static_cast<double>(uintValue);
							}
							else
							{
								doubleValue = static_cast<double>(intValue);
							}
						}
						else
						{
							// invalid digit
							digit = intBase;
							break;
						}
						state = DoubleFractional;
					}
					else
					{
						// invalid digit
						digit = intBase;
						break;
					}

					continue;

				case EOF:
				default:
					if(c != EOF)
					{
						stream.unget();
					}
					// invalid digit
					digit = intBase;
					break;

				}

				if(digit >= intBase)
				{
					// invalid digit
					if(digits == 0)
					{
						stream.setState(std::ios_base::failbit);
					}
					else
					{
						if(state == Int)
						{
							if(sign > 0)
							{
								value = uintValue;
							}
							else
							{
								value = intValue;
							}
						}
						else // if(state == DoubleExponent)
						{
							if(sign > 0)
							{
								value = doubleValue * pow(10.0, uintValue);
							}
							else
							{
								value = doubleValue * pow(10.0, intValue);
							}
						}
					}
					return stream;
				}

				bool overflow = false;
				if(sign > 0)
				{
					uint64_t v = uintValue * intBase + digit;
					if(v >= uintValue || intBase != 10)
					{
						uintValue = v;
					}
					else
					{
						overflow = true;
					}
				}
				else
				{
					int64_t v = intValue * intBase - digit;
					if(v <= intValue || intBase != 10)
					{
						intValue = v;
					}
					else
					{
						overflow = true;
					}
				}

				if(overflow)
				{
					// number is too large
					if(state == Int)
					{
						// fallback to double
						if(c != EOF)
						{
							stream.unget();
						}
						if(sign > 0)
						{
							doubleValue = static_cast<double>(uintValue);
						}
						else
						{
							doubleValue = static_cast<double>(intValue);
						}
						state = Double;
						continue;
					}
					else // if(state == DoubleExponent)
					{
						stream.setState(std::ios_base::failbit);
						return stream;
					}
				}
			}
			continue;

		case Double:
			switch(c)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					int digit = c - '0';
					digits += 1;
					doubleValue = doubleValue * 10.0 + static_cast<double>(digit * sign);
				}
				continue;

			case 'e':
			case 'E':
				state = DoubleExponent;

				// reset int state
				sign = 1;
				hadSign = false;
				uintValue = 0;
				intValue = 0;
				intBase = 10;
				digits = 0;

				continue;

			case '.':
			case ',':
				// fractional separator
				state = DoubleFractional;
				continue;

			case EOF:
			default:
				// invalid digit
				if(digits == 0)
				{
					stream.setState(std::ios_base::failbit);
				}
				else
				{
					if(c != EOF)
					{
						stream.unget();
					}
					value = doubleValue;
				}
				return stream;

			}
			continue;

		case DoubleFractional:
			switch(c)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					int digit = c - '0';
					digits += 1;
					doubleFactor /= 10.0;
					doubleValue += static_cast<double>(digit * sign) * doubleFactor;
				}
				break;

			case 'e':
			case 'E':
				state = DoubleExponent;

				// reset int state
				sign = 1;
				hadSign = false;
				uintValue = 0;
				intValue = 0;
				intBase = 10;
				digits = 0;

				continue;

			case EOF:
			default:
				// invalid digit
				if(digits == 0)
				{
					stream.setState(std::ios_base::failbit);
				}
				else
				{
					if(c != EOF)
					{
						stream.unget();
					}
					value = doubleValue;
				}
				return stream;

			}
			break;
		}
	}

	return stream;
}


BinaryStream& operator<<( BinaryStream& stream, const Variant& value )
{
	value.type()->streamOut( stream, value.payload() );
	return stream;
}


BinaryStream& operator>>( BinaryStream& stream, Variant& value )
{
	value.type()->streamIn( stream, value.payload() );
	return stream;
}


std::ostream& operator<<( std::ostream& stream, const Variant& value )
{
	StdDataStream dataStream( stream.rdbuf() );
	TextStream textStream( dataStream );
	textStream << value;
	stream.setstate( textStream.state() );
	return stream;
}


std::istream& operator>>( std::istream& stream, Variant& value )
{
	StdDataStream dataStream( stream.rdbuf() );
	TextStream textStream( dataStream );
	textStream >> value;
	stream.setstate( textStream.state() );
	return stream;
}
} // end namespace wgt

#endif
