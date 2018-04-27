#include "variant.hpp"

#include "core_common/assert.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/std_data_stream.hpp"

#include <stdexcept>

#include <cstring>
#include <cmath>
#include <cctype>
#include <cstdio>

namespace wgt
{
namespace
{
bool wtoutf8(const wchar_t* wsrc, std::string& output)
{
	if (wsrc == nullptr)
	{
		output.clear();
		return true;
	}

	output = StringUtils::to_string(wsrc);
	return true;
}

bool utf8tow(const char* src, std::wstring& output)
{
	if (src == nullptr)
	{
		output.clear();
		return true;
	}

	output = StringUtils::to_wstring(src);
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
	wtoutf8(v.c_str(), str);
	return str;
}

std::string upcast(const wchar_t* v)
{
	std::string str;
	wtoutf8(v, str);
	return str;
	;
}

bool downcast(std::wstring* v, const std::string& storage)
{
	if (v)
	{
		utf8tow(storage.c_str(), *v);
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

Variant::COWData* Variant::COWData::allocate(size_t payloadSize)
{
	char* data = new char[sizeof(COWData) + payloadSize];
	return new (data) COWData();
}

//------------------------------------------------------------------------------
Variant::COWData::COWData() : refs_(0)
{
}

//------------------------------------------------------------------------------
bool Variant::COWData::isExclusive() const
{
	return refs_ == 1;
}

//------------------------------------------------------------------------------
void* Variant::COWData::payload()
{
	return this + 1;
}

//------------------------------------------------------------------------------
const void* Variant::COWData::payload() const
{
	return this + 1;
}

//------------------------------------------------------------------------------
void Variant::COWData::incRef()
{
	refs_.fetch_add(1);
}

//------------------------------------------------------------------------------
void Variant::COWData::decRef(const MetaType* type)
{
	if (refs_.fetch_sub(1) == 1)
	{
		TF_ASSERT(type);
		type->destroy(payload());
		this->~COWData();
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

Variant::Variant(const MetaType* type)
{
	TF_ASSERT(type);

	auto qualifiedType = type->qualified(0);
	void* p;
	if (isInline(type))
	{
		setTypeInternal(qualifiedType, Inline);
		p = data_.inline_;
	}
	else
	{
		setTypeInternal(qualifiedType, COW);
		data_.cow_ = COWData::allocate(type->size());
		p = data_.cow()->payload();
		data_.cow()->incRef();
	}

	type->init(p);
}

Variant::Variant(const MetaType* type, const Variant& value)
{
	if (!convertInit(type, value))
	{
		castError();
	}
}

Variant::Variant(const MetaType* type, const Variant& value, bool* succeeded)
{
	bool r = convertInit(type, value);
	if (!r)
	{
		initVoid();
	}

	if (succeeded)
	{
		*succeeded = r;
	}
}

Variant& Variant::operator=(const Variant& value)
{
	if (this == &value)
	{
		return *this;
	}

	if (type_ == value.type_)
	{
		// both types and storages match
		switch (storageKind())
		{
		case Inline:
			type()->copy(data_.inline_, value.data_.inline_);
			break;

		case RawPointer:
			data_.rawPointer_ = value.data_.rawPointer_;
			break;

		case SharedPointer:
			data_.sharedPointer() = value.data_.sharedPointer();
			break;

		case COW:
			if (data_.cow_ != value.data_.cow_)
			{
				data_.cow_->decRef(type());
				data_.cow_ = value.data_.cow_;
				data_.cow_->incRef();
			}
			break;

		default:
			TF_ASSERT(false);
			break;
		}
	}
	else
	{
		// common case
		destroy();
		init(value);
	}

	return *this;
}

Variant& Variant::operator=(Variant&& value)
{
	if (this == &value)
	{
		return *this;
	}

	if (type_ == value.type_)
	{
		// both types and storages match
		switch (storageKind())
		{
		case Inline:
			type()->move(data_.inline_, value.data_.inline_);
			break;

		case RawPointer:
			data_.rawPointer_ = value.data_.rawPointer_;
			break;

		case SharedPointer:
			data_.sharedPointer() = std::move(value.data_.sharedPointer());
			break;

		case COW:
			if (data_.cow_ != value.data_.cow_)
			{
				data_.cow_->decRef(type());
				data_.cow_ = value.data_.cow_;
				data_.cow_->incRef();
			}
			break;

		default:
			TF_ASSERT(false);
			break;
		}
	}
	else
	{
		// common case
		destroy();
		init(std::move(value));
	}

	return *this;
}

bool Variant::operator<(const Variant& that) const
{
	if (this == &that)
	{
		return false;
	}

	auto thisType = type();
	auto thatType = that.type();

	if (thisType == thatType)
	{
		auto lp = value<const void*>();
		auto rp = that.value<const void*>();

		return lp != rp && thisType->lessThan(lp, rp);
	}

	bool succeeded = false;
	Variant tmp = that.convert(thisType, &succeeded);
	if (!succeeded)
	{
		tmp = this->convert(thatType, &succeeded);
		if (!succeeded)
		{
			return thisType < thatType;
		}

		auto lp = tmp.value<const void*>();
		auto rp = that.value<const void*>();

		return thatType->lessThan(lp, rp);
	}

	auto lp = value<const void*>();
	auto rp = tmp.value<const void*>();

	return thisType->lessThan(lp, rp);
}

bool Variant::operator==(const Variant& that) const
{
	if (this == &that)
	{
		return true;
	}

	auto thisType = type();
	auto thatType = that.type();

	if (thisType == thatType)
	{
		auto lp = value<const void*>();
		auto rp = that.value<const void*>();

		return lp == rp || thisType->equal(lp, rp);
	}

	bool succeeded = false;
	Variant tmp = that.convert(thisType, &succeeded);
	if (!succeeded)
	{
		return false;
	}

	auto lp = value<const void*>();
	auto rp = tmp.value<const void*>();

	return thisType->equal(lp, rp);
}

uint64_t Variant::getHashCode() const
{
	if (type() == nullptr)
	{
		return 0;
	}
	return type()->hashCode( value<const void*>() );
}

bool Variant::setType(const MetaType* type)
{
	if (type == this->type())
	{
		return true;
	}

	bool succeeded = false;
	Variant tmp(type, *this, &succeeded);
	if (!succeeded)
	{
		return false;
	}

	*this = std::move(tmp);

	return true;
}

bool Variant::isVoid() const
{
	return typeIs<void>() && !isPointer();
}

bool Variant::isPointer() const
{
	switch (storageKind())
	{
	case RawPointer:
	case SharedPointer:
		return true;

	case Inline:
	case COW: // note that COW acts as value, so we don't consider it as pointer
		return false;

	default:
		TF_ASSERT(false);
		return false;
	};
}

bool Variant::isNullPointer() const
{
	switch (storageKind())
	{
	case RawPointer:
		return data_.rawPointer_ == nullptr;

	case SharedPointer:
		return data_.sharedPointer().get() == nullptr;

	case Inline:
	case COW: // note that COW acts as value, so we don't consider it as pointer
		return false;

	default:
		TF_ASSERT(false);
		return false;
	};
}

void Variant::initVoid()
{
	setTypeInternal(getQualifiedType<void>(), Inline);

	// no payload initialization
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
	TF_ASSERT(type_);

	switch (storageKind())
	{
	case Inline:
	{
		auto thisType = type();
		thisType->init(data_.inline_);
		thisType->copy(data_.inline_, value.data_.inline_);
	}
	break;

	case RawPointer:
		data_.rawPointer_ = value.data_.rawPointer_;
		break;

	case SharedPointer:
		new (data_.sharedPointer_) std::shared_ptr<void>(value.data_.sharedPointer());
		break;

	case COW:
		data_.cow_ = value.data_.cow_;
		data_.cow_->incRef();
		break;
	}
}

void Variant::init(Variant&& value)
{
	type_ = value.type_;
	TF_ASSERT(type_);

	switch (storageKind())
	{
	case Inline:
	{
		auto thisType = type();
		thisType->init(data_.inline_);
		thisType->move(data_.inline_, value.data_.inline_);
	}
	break;

	case RawPointer:
		data_.rawPointer_ = value.data_.rawPointer_;
		break;

	case SharedPointer:
		new (data_.sharedPointer_) std::shared_ptr<void>(std::move(value.data_.sharedPointer()));
		break;

	case COW:
		// Since we are moving the data, there is no need to
		// change the ref counts, just transfer ownership across
		data_.cow_ = value.data_.cow_;
		// Clear out the moved value, so successive moves don't work
		value.initVoid();
		value.data_.cow_ = nullptr;
		break;
	}
}

bool Variant::convertInit(const MetaType* type, const Variant& that)
{
	if (!type)
	{
		return false;
	}

	if (type == that.type())
	{
		init(that);
		return true;
	}

	auto qualifiedType = type->qualified(0);

	void* p;
	if (isInline(type))
	{
		setTypeInternal(qualifiedType, Inline);
		p = data_.inline_;
	}
	else
	{
		setTypeInternal(qualifiedType, COW);
		data_.cow_ = COWData::allocate(type->size());
		p = data_.cow()->payload();
		data_.cow()->incRef();
	}

	type->init(p);

	if (!type->convertFrom(p, that.type(), that.value<const void*>()))
	{
		destroy();
		return false;
	}

	return true;
}

/**
Destroy currently held value and free all external resources used by it.
@warning This function leaves variant in uninitialized/undefined state and
breaks invariants, so it should be used with care.
@see init
*/
void Variant::destroy()
{
	switch (storageKind())
	{
	case Inline:
		type()->destroy(data_.inline_);
		break;

	case RawPointer:
		// we do not own the pointer, so no-op
		break;

	case SharedPointer:
		data_.sharedPointer().~shared_ptr();
		break;

	case COW:
		data_.cow_->decRef(type());
		break;
	}
}

/**
Ensure we hold an exclusive payload instance.

If actual detach happens then @a copy parameter specifies whether old value
should be copied to a new one. Otherwise new value is left default-initialized.
*/
void Variant::detach(bool copy)
{
	TF_ASSERT(storageKind() == COW);
	if (data_.cow_->isExclusive())
	{
		return;
	}

	// allocate and initialize new payload copy
	auto thisType = type();
	COWData* newCow = COWData::allocate(thisType->size());
	thisType->init(newCow->payload());
	if (copy)
	{
		thisType->copy(newCow->payload(), data_.cow()->payload());
	}

	// substitute payload with the new one (which is obviously exclusive)
	data_.cow_->decRef(thisType);
	data_.cow_ = newCow;
}

void Variant::castError()
{
#ifdef _WIN32
#if (_MSC_VER >= 1900)
	throw std::bad_cast::__construct_from_string_literal("Variant cast failed");
#else
	throw std::bad_cast("Variant cast failed");
#endif
#else
	throw std::bad_cast();
#endif
}

////////////////////////////////////////////////////////////////////////////////

TextStream& operator<<(TextStream& stream, StrongType<const Variant&> value)
{
	value.value().type()->streamOut(stream, value.value().value<const void*>());
	return stream;
}

TextStream& operator>>(TextStream& stream, Variant& value)
{
	if (!stream.beginReadField())
	{
		return stream;
	}

	if (!value.isVoid())
	{
		if (auto ptr = value.value<const void*>())
		{
			value.type()->streamIn(stream, const_cast<void*>(ptr));
			stream.peek();
		}
		else
		{
			// can't modify value (is it const?)
			stream.setState(std::ios_base::failbit);
		}
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

	while (stream.good())
	{
		const int c = stream.get();
		switch (state)
		{
		case UnknownType:
			switch (c)
			{
			case EOF:
				// unexpected end of stream
				stream.setState(std::ios_base::failbit);
				return stream;

			case '"': // string
			{
				stream.unget();
				auto stringType = MetaType::get<std::string>();
				TF_ASSERT(stringType);
				Variant tmp(stringType);
				auto ptr = tmp.value<void*>();
				TF_ASSERT(ptr);
				stringType->streamIn(stream, ptr);
				if (!stream.fail())
				{
					value = std::move(tmp);
				}
			}
				// calling peek here is trying to set eofbit
				// if next value in stream reaches EOF
				stream.peek();
				return stream;

			case 'v':
			{
				stream.unget();
				auto voidType = MetaType::get<void>();
				TF_ASSERT(voidType);
				Variant tmp(voidType);
				auto ptr = tmp.value<void*>();
				TF_ASSERT(ptr);
				voidType->streamIn(stream, ptr);
				if (!stream.fail())
				{
					value = std::move(tmp);
				}
			}
				// calling peek here is trying to set eofbit
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
			switch (tolower(c))
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
				if (c != EOF)
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
			switch (l)
			{
			case '-':
			case '+':
				if (!hadSign && digits == 0)
				{
					hadSign = true;
					if (l == '-')
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
				if (intBase == 10)
				{
					if (state == Int)
					{
						if (sign > 0)
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
				if (intBase == 10)
				{
					if (state == Int)
					{
						if (sign > 0)
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
				if (c != EOF)
				{
					stream.unget();
				}
				// invalid digit
				digit = intBase;
				break;
			}

			if (digit >= intBase)
			{
				// invalid digit
				if (digits == 0)
				{
					stream.setState(std::ios_base::failbit);
				}
				else
				{
					if (state == Int)
					{
						if (sign > 0)
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
						if (sign > 0)
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
			if (sign > 0)
			{
				uint64_t v = uintValue * intBase + digit;
				if (v >= uintValue || intBase != 10)
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
				if (v <= intValue || intBase != 10)
				{
					intValue = v;
				}
				else
				{
					overflow = true;
				}
			}

			if (overflow)
			{
				// number is too large
				if (state == Int)
				{
					// fallback to double
					if (c != EOF)
					{
						stream.unget();
					}
					if (sign > 0)
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
			switch (c)
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
				if (digits == 0)
				{
					stream.setState(std::ios_base::failbit);
				}
				else
				{
					if (c != EOF)
					{
						stream.unget();
					}
					value = doubleValue;
				}
				return stream;
			}
			continue;

		case DoubleFractional:
			switch (c)
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
				if (digits == 0)
				{
					stream.setState(std::ios_base::failbit);
				}
				else
				{
					if (c != EOF)
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

BinaryStream& operator<<(BinaryStream& stream, StrongType<const Variant&> value)
{
	value.value().type()->streamOut(stream, value.value().value<const void*>());
	return stream;
}

BinaryStream& operator>>(BinaryStream& stream, Variant& value)
{
	if (auto ptr = value.value<void*>())
	{
		value.type()->streamIn(stream, ptr);
	}
	else
	{
		// cant' modify value (is it const?)
		stream.setState(std::ios_base::failbit);
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, StrongType<const Variant&> value)
{
	StdDataStream dataStream(stream.rdbuf());
	TextStream textStream(dataStream);
	textStream << value.value();
	stream.setstate(textStream.state());
	return stream;
}

std::istream& operator>>(std::istream& stream, Variant& value)
{
	StdDataStream dataStream(stream.rdbuf());
	TextStream textStream(dataStream);
	textStream >> value;
	stream.setstate(textStream.state());
	return stream;
}
} // end namespace wgt
