#include "pch.hpp"

#include <string>
#include <sstream>
#include <limits>
#include <memory>
#include <cstring>
#include <cmath>
#include <bitset>

#include "core_variant/variant.hpp"

#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"

#define EXTRA_ARGS_DECLARE TestResult &result_, const char *m_name
#define EXTRA_ARGS result_, m_name

namespace wgt
{
namespace
{
struct Base
{
	explicit Base(int i = 0) : i(i), f(0.5f), s("hello")
	{
	}

	virtual ~Base()
	{
	}

	int i;
	float f;
	std::string s;

	bool operator==(const Base& v) const
	{
		return i == v.i && f == v.f && s == v.s;
	}

	bool operator!=(const Base& v) const
	{
		return !(*this == v);
	}

	Base& operator=(const Base& that)
	{
		i = that.i;
		f = that.f;
		s = that.s;

		return *this;
	}
};

Base* upcast(Base* v)
{
	return v;
}

template <typename T>
typename std::enable_if<std::is_base_of<Base, T>::value, bool>::type downcast(T** v, Base* storage)
{
	if (T* tmp = dynamic_cast<T*>(storage))
	{
		if (v)
		{
			*v = tmp;
		}

		return true;
	}

	return false;
}

template <int I>
struct Derived : Base
{
	Derived() : Base(I)
	{
	}
};

template <int I>
struct DerivedPtr
{
	typedef Derived<I> D;
	explicit DerivedPtr(D* ptr = nullptr) : ptr(ptr)
	{
	}

	bool operator==(const DerivedPtr& that) const
	{
		return ptr == that.ptr;
	}

	D* ptr;
};

template <int I>
Derived<I>* upcast(const DerivedPtr<I>& v)
{
	return v.ptr;
}

template <int I>
bool downcast(DerivedPtr<I>* v, Derived<I>* storage)
{
	if (v)
	{
		v->ptr = storage;
	}

	return true;
}

TextStream& operator<<(TextStream& stream, const Base& v)
{
	stream << v.i << ' ' << v.f << ' ' << quoted(v.s);
	return stream;
}

TextStream& operator>>(TextStream& stream, Base& v)
{
	stream >> v.i >> v.f >> quoted(v.s);
	return stream;
}

////////////////////////////////////////////////////////////////////////////

bool areEqual(float lhs, float rhs)
{
	return std::abs(lhs - rhs) <= 1e-5f;
}

bool areEqual(double lhs, double rhs)
{
	return std::abs(lhs - rhs) <= 1e-13f;
}

bool areEqual(double lhs, float rhs)
{
	return areEqual(static_cast<float>(lhs), rhs);
}

bool areEqual(const char* lhs, const char* rhs)
{
	return lhs == rhs || strcmp(lhs, rhs) == 0;
}

template <typename T1, typename T2>
bool areEqual(const T1& lhs, const T2& rhs)
{
	return lhs == rhs;
}

template <typename T, typename Check>
void variantCheck(EXTRA_ARGS_DECLARE, const Variant& v, const Check& check)
{
	// type
	CHECK(v.typeIs<T>());
	CHECK(!v.isVoid());

	// everything can be casted to `const void*`, i.e. to its storage address
	CHECK(v.canCast<const void*>());
	CHECK(v.cast<const void*>());

	// cast
	CHECK(areEqual(v.cast<T>(), check));
	CHECK(areEqual(v, check));

	// tryCast
	{
		T tmp;
		CHECK(v.tryCast(tmp));
		CHECK(areEqual(tmp, check));
	}

	// value
	CHECK(areEqual(v.value<T>(), check));

	// marshaling
	{
		Variant tmp = v.cast<T>();
		CHECK(areEqual(tmp, v));
		CHECK(areEqual(tmp, check));
	}

	// to/from string conversion
	{
		Variant tmp = v.cast<std::string>();
		// note that v.cast<T>() will fail here since it can't return const-ref to an object
		CHECK(areEqual(tmp.value<T>(), check));
		CHECK(areEqual(tmp, check));
		CHECK(areEqual(v, tmp));
		CHECK(areEqual(tmp, v));
	}
}

template <typename T>
void serializationCheck(EXTRA_ARGS_DECLARE, const Variant& v, const char* serialized, const T& check)
{
	ResizingMemoryStream dataStream;
	TextStream s(dataStream);
	s << v;
	CHECK(s.good());
	s.sync();

	CHECK_EQUAL(serialized, dataStream.buffer());

	Variant tmp = T(); // give type to deserializer
	s.seek(0);
	s >> tmp;

	RETURN_ON_FAIL_CHECK(!s.fail());

	CHECK(areEqual(tmp.cast<T>(), check));
	CHECK(areEqual(tmp, check));
	CHECK(areEqual(v, tmp));
	CHECK(areEqual(tmp, v));
}

template <typename T, typename Check>
void variantCheck(EXTRA_ARGS_DECLARE, const Variant& v, const Check& check, const char* serialized)
{
	variantCheck<T, Check>(EXTRA_ARGS, v, check);
	serializationCheck<T>(EXTRA_ARGS, v, serialized, static_cast<T>(check));
}

template <typename T, typename Check>
void castCheck(EXTRA_ARGS_DECLARE, const Variant& v, const Check& check)
{
	CHECK(v.canCast<T>());
	CHECK(areEqual(v.cast<T>(), check));
	CHECK(areEqual(v, check));

	T tmp;
	CHECK(v.tryCast(tmp));
	CHECK(areEqual(tmp, check));

	CHECK(areEqual(v.value<T>(), check));
}

template <typename T>
void castFailCheck(EXTRA_ARGS_DECLARE, const Variant& v)
{
	CHECK(!v.canCast<T>());

	T tmp = T();
	CHECK(!v.tryCast(tmp));

	CHECK(areEqual(v.value<T>(), T()));
}

template <typename T>
void deserializeCheck(EXTRA_ARGS_DECLARE, const char* str, const T& check)
{
	FixedMemoryStream dataStream(str);
	TextStream s(dataStream);
	Variant v;
	s >> v;

	CHECK(v.typeIs<T>());
	CHECK(areEqual(v.cast<T>(), check));
	CHECK(areEqual(v, check));
}
}

TEST(Variant_construct)
{
	Variant v;
	CHECK(v.typeIs<void>());
	CHECK(v.isVoid());

	ResizingMemoryStream dataStream;
	TextStream s(dataStream);
	s << v;
	CHECK(s.good());
	s.sync();

	CHECK_EQUAL("void", dataStream.buffer());

	Variant tmp;
	s.seek(0);
	s >> tmp;
	CHECK(!s.fail());

	CHECK(tmp.typeIs<void>());
	CHECK(tmp.isVoid());

	CHECK(areEqual(tmp, v));
	CHECK(areEqual(v, tmp));
}

TEST(Variant_int)
{
	Variant v = 42;
	variantCheck<int32_t>(EXTRA_ARGS, v, 42, "42");

	castCheck<int64_t>(EXTRA_ARGS, v, 42);
	castCheck<int32_t>(EXTRA_ARGS, v, 42);
	castCheck<int16_t>(EXTRA_ARGS, v, 42);
	castCheck<int8_t>(EXTRA_ARGS, v, 42);
	castCheck<uint64_t>(EXTRA_ARGS, v, 42);
	castCheck<uint32_t>(EXTRA_ARGS, v, 42);
	castCheck<uint16_t>(EXTRA_ARGS, v, 42);
	castCheck<uint8_t>(EXTRA_ARGS, v, 42);
	castCheck<double>(EXTRA_ARGS, v, 42.0);
	castCheck<float>(EXTRA_ARGS, v, 42.0f);
	castCheck<std::string>(EXTRA_ARGS, v, "42");
}

TEST(Variant_int64)
{
	Variant v = (int64_t)42;
	variantCheck<int64_t>(EXTRA_ARGS, v, 42, "42");

	castCheck<int64_t>(EXTRA_ARGS, v, 42);
	castCheck<int32_t>(EXTRA_ARGS, v, 42);
	castCheck<int16_t>(EXTRA_ARGS, v, 42);
	castCheck<int8_t>(EXTRA_ARGS, v, 42);
	castCheck<uint64_t>(EXTRA_ARGS, v, 42);
	castCheck<uint32_t>(EXTRA_ARGS, v, 42);
	castCheck<uint16_t>(EXTRA_ARGS, v, 42);
	castCheck<uint8_t>(EXTRA_ARGS, v, 42);
	castCheck<double>(EXTRA_ARGS, v, 42.0);
	castCheck<float>(EXTRA_ARGS, v, 42.0f);
	castCheck<std::string>(EXTRA_ARGS, v, "42");

	v = 9223372036854775807;
	variantCheck<int64_t>(EXTRA_ARGS, v, 9223372036854775807, "9223372036854775807");
	castCheck<int64_t>(EXTRA_ARGS, v, 9223372036854775807);
	castCheck<uint64_t>(EXTRA_ARGS, v, 9223372036854775807);
	castCheck<double>(EXTRA_ARGS, v, 9223372036854775807.0);
}

TEST(Variant_double)
{
	Variant v = 1.5;
	variantCheck<double>(EXTRA_ARGS, v, 1.5, "1.5");

	castCheck<int64_t>(EXTRA_ARGS, v, 1);
	castCheck<int32_t>(EXTRA_ARGS, v, 1);
	castCheck<int16_t>(EXTRA_ARGS, v, 1);
	castCheck<int8_t>(EXTRA_ARGS, v, 1);
	castCheck<uint64_t>(EXTRA_ARGS, v, 1);
	castCheck<uint32_t>(EXTRA_ARGS, v, 1);
	castCheck<uint16_t>(EXTRA_ARGS, v, 1);
	castCheck<uint8_t>(EXTRA_ARGS, v, 1);
	castCheck<double>(EXTRA_ARGS, v, 1.5);
	castCheck<float>(EXTRA_ARGS, v, 1.5f);
	castCheck<std::string>(EXTRA_ARGS, v, "1.5");
}

TEST(Variant_bool)
{
	Variant v = true;
	variantCheck<int32_t>(EXTRA_ARGS, v, 1, "1");

	CHECK(v.cast<bool>() == true);
	CHECK(v == true);

	castCheck<uint64_t>(EXTRA_ARGS, v, 1);
	castCheck<int32_t>(EXTRA_ARGS, v, 1);
	castCheck<int16_t>(EXTRA_ARGS, v, 1);
	castCheck<int8_t>(EXTRA_ARGS, v, 1);
	castCheck<uint64_t>(EXTRA_ARGS, v, 1);
	castCheck<uint32_t>(EXTRA_ARGS, v, 1);
	castCheck<uint16_t>(EXTRA_ARGS, v, 1);
	castCheck<uint8_t>(EXTRA_ARGS, v, 1);
	castCheck<double>(EXTRA_ARGS, v, 1.0);
	castCheck<float>(EXTRA_ARGS, v, 1.0f);
	castCheck<std::string>(EXTRA_ARGS, v, "1");
}

TEST(Variant_bit_set)
{
	std::bitset<8> bitset;
	bitset.set(1);
	bitset.set(3);
	bitset.set(5);
	bitset.set(7);
	Variant v = bitset;
	variantCheck<std::bitset<8>>(EXTRA_ARGS, v, bitset);
}

TEST(Variant_string_number)
{
	Variant v = "-1.5";
	variantCheck<std::string>(EXTRA_ARGS, v, "-1.5", "\"-1.5\"");

	// successful but partial conversion (i.e. data loss) is failure
	castFailCheck<int64_t>(EXTRA_ARGS, v);
	castFailCheck<int32_t>(EXTRA_ARGS, v);
	castFailCheck<int16_t>(EXTRA_ARGS, v);
	castFailCheck<int8_t>(EXTRA_ARGS, v);

	// storing negative values in unsigned storage is a bit tricky, so don't test it
	// castCheck<uint64_t>(EXTRA_ARGS, v, (uint64_t)-123);
	// castCheck<uint32_t>(EXTRA_ARGS, v, (uint32_t)-123);
	// castCheck<uint16_t>(EXTRA_ARGS, v, (uint16_t)-123);
	// castCheck<uint8_t>(EXTRA_ARGS, v, (uint8_t)-123);

	castCheck<double>(EXTRA_ARGS, v, -1.5);
	castCheck<float>(EXTRA_ARGS, v, -1.5f);

	castCheck<std::string>(EXTRA_ARGS, v, "-1.5");
}

TEST(Variant_string)
{
	const char buf[] = { 'q', 'w', 'e', '\\', '"', '\0', '\r', '\n' };
	const std::string s(buf, buf + sizeof(buf) / sizeof(buf[0]));

	Variant v = s;
	variantCheck<std::string>(EXTRA_ARGS, v, s, "\"qwe\\\\\\\"\\0\\r\\n\"");

	castFailCheck<int64_t>(EXTRA_ARGS, v);
	castFailCheck<int32_t>(EXTRA_ARGS, v);
	castFailCheck<int16_t>(EXTRA_ARGS, v);
	castFailCheck<int8_t>(EXTRA_ARGS, v);
	castFailCheck<uint64_t>(EXTRA_ARGS, v);
	castFailCheck<uint32_t>(EXTRA_ARGS, v);
	castFailCheck<uint16_t>(EXTRA_ARGS, v);
	castFailCheck<uint8_t>(EXTRA_ARGS, v);
	castFailCheck<double>(EXTRA_ARGS, v);
	castFailCheck<float>(EXTRA_ARGS, v);

	castCheck<std::string>(EXTRA_ARGS, v, s);
}

TEST(Variant_custom_type)
{
	Variant v = Base(42);
	variantCheck<Base>(EXTRA_ARGS, v, Base(42), "42 0.5 \"hello\"");

	castFailCheck<int64_t>(EXTRA_ARGS, v);
	castFailCheck<int32_t>(EXTRA_ARGS, v);
	castFailCheck<int16_t>(EXTRA_ARGS, v);
	castFailCheck<int8_t>(EXTRA_ARGS, v);
	castFailCheck<uint64_t>(EXTRA_ARGS, v);
	castFailCheck<uint32_t>(EXTRA_ARGS, v);
	castFailCheck<uint16_t>(EXTRA_ARGS, v);
	castFailCheck<uint8_t>(EXTRA_ARGS, v);
	castFailCheck<double>(EXTRA_ARGS, v);
	castFailCheck<float>(EXTRA_ARGS, v);

	castCheck<std::string>(EXTRA_ARGS, v, "42 0.5 \"hello\"");

	castCheck<Base>(EXTRA_ARGS, v, Base(42));

	CHECK(v.cast<Base>() != Base(1));
}

TEST(Variant_qualifiers)
{
	const Base b(42);
	Variant v = &b;

	variantCheck<Base>(EXTRA_ARGS, v, Base(42), "42 0.5 \"hello\"");

	castFailCheck<int64_t>(EXTRA_ARGS, v);
	castFailCheck<int32_t>(EXTRA_ARGS, v);
	castFailCheck<int16_t>(EXTRA_ARGS, v);
	castFailCheck<int8_t>(EXTRA_ARGS, v);
	castFailCheck<uint64_t>(EXTRA_ARGS, v);
	castFailCheck<uint32_t>(EXTRA_ARGS, v);
	castFailCheck<uint16_t>(EXTRA_ARGS, v);
	castFailCheck<uint8_t>(EXTRA_ARGS, v);
	castFailCheck<double>(EXTRA_ARGS, v);
	castFailCheck<float>(EXTRA_ARGS, v);

	castCheck<std::string>(EXTRA_ARGS, v, "42 0.5 \"hello\"");

	castCheck<Base>(EXTRA_ARGS, v, Base(42));

	CHECK(v.cast<Base>() != Base(1));

	CHECK(!v.canCast<Base&>());
	CHECK(v.canCast<const Base&>());
	CHECK(!v.canCast<volatile Base&>());
	CHECK(v.canCast<const volatile Base&>());
}

/*TEST(Variant_qualified_upcast)
{
    const Derived<42> d;
    Variant v = &d;

    variantCheck<Base>(EXTRA_ARGS, v, Base(42), "42 0.5 \"hello\"");

    castFailCheck<int64_t>(EXTRA_ARGS, v);
    castFailCheck<int32_t>(EXTRA_ARGS, v);
    castFailCheck<int16_t>(EXTRA_ARGS, v);
    castFailCheck<int8_t>(EXTRA_ARGS, v);
    castFailCheck<uint64_t>(EXTRA_ARGS, v);
    castFailCheck<uint32_t>(EXTRA_ARGS, v);
    castFailCheck<uint16_t>(EXTRA_ARGS, v);
    castFailCheck<uint8_t>(EXTRA_ARGS, v);
    castFailCheck<double>(EXTRA_ARGS, v);
    castFailCheck<float>(EXTRA_ARGS, v);

    castCheck<std::string>(EXTRA_ARGS, v, "42 0.5 \"hello\"");

    castCheck<Base>(EXTRA_ARGS, v, Base(42));

    CHECK(v.cast<Base>() != Base(1));

    CHECK(!v.canCast<Base&>());
    CHECK(v.canCast<const Base&>());
    CHECK(!v.canCast<volatile Base&>());
    CHECK(v.canCast<const volatile Base&>());
}*/

TEST(Variant_void_pointer)
{
	int i = 42;
	void* p = &i;

	Variant v = p;

	CHECK(v.typeIs<void>());
	CHECK(!v.isVoid());
	CHECK(v.isPointer());
	CHECK(!v.isNullPointer());

	CHECK(v.cast<void*>() == &i);
	CHECK(v.cast<const void*>() == &i);
	CHECK(v.cast<volatile void*>() == &i);
	CHECK(v.cast<const volatile void*>() == &i);

	// marshaling
	{
		Variant tmp = v.cast<void*>();
		CHECK(tmp == v);
		CHECK(tmp == p);
	}
}

TEST(Variant_const_void_pointer)
{
	const void* p = nullptr;

	Variant v = p;

	CHECK(v.typeIs<void>());
	CHECK(!v.isVoid());
	CHECK(v.isPointer());
	CHECK(v.isNullPointer());

	CHECK(!v.canCast<void*>());
	CHECK(v.cast<const void*>() == nullptr);
	CHECK(!v.canCast<volatile void*>());
	CHECK(v.cast<const volatile void*>() == nullptr);

	// marshaling
	{
		Variant tmp = v.cast<const void*>();
		CHECK(tmp == v);
		CHECK(tmp == p);
	}
}

TEST(Variant_pointer)
{
	Base tmp(42);

	Variant v = &tmp;
	variantCheck<Base>(EXTRA_ARGS, v, Base(42), "42 0.5 \"hello\"");

	CHECK(v.isPointer());
	CHECK(!v.isNullPointer());

	castFailCheck<int64_t>(EXTRA_ARGS, v);
	castFailCheck<int32_t>(EXTRA_ARGS, v);
	castFailCheck<int16_t>(EXTRA_ARGS, v);
	castFailCheck<int8_t>(EXTRA_ARGS, v);
	castFailCheck<uint64_t>(EXTRA_ARGS, v);
	castFailCheck<uint32_t>(EXTRA_ARGS, v);
	castFailCheck<uint16_t>(EXTRA_ARGS, v);
	castFailCheck<uint8_t>(EXTRA_ARGS, v);
	castFailCheck<double>(EXTRA_ARGS, v);
	castFailCheck<float>(EXTRA_ARGS, v);

	castCheck<Base*>(EXTRA_ARGS, v, &tmp);
	castCheck<void*>(EXTRA_ARGS, v, &tmp);
	castCheck<const void*>(EXTRA_ARGS, v, &tmp);
}

TEST(Variant_cast)
{
	Derived<1> tmp;
	Base* tmp_b = &tmp;

	// upcast Derived<1>* to Base*
	Variant v = &tmp;
	variantCheck<Base>(EXTRA_ARGS, v, *tmp_b);
	CHECK(v.isPointer());
	CHECK(v.cast<void*>() == tmp_b);

	// downcast Base* to Derived<1>*
	CHECK(v.cast<Derived<1>*>() == &tmp);
	CHECK(!v.canCast<Derived<2>*>());

	// conversion to/from string
	Variant vs = v.cast<std::string>();
	CHECK(vs.cast<Base>() == tmp);
	CHECK(!vs.canCast<Base&>());
	CHECK(vs.setType<Base>());
	CHECK(vs.cast<Base&>() == tmp);
}

TEST(Variant_recursive_cast)
{
	Derived<1> d;
	typedef DerivedPtr<1> Ptr;
	Ptr tmp(&d);

	// upcast DerivedPtr<1> -> Derived<1>* -> Base*
	Variant v = tmp;
	variantCheck<Base>(EXTRA_ARGS, v, Base(1));
	CHECK(v.isPointer());
	CHECK(v.cast<void*>() == tmp.ptr);
	CHECK(v.cast<Base*>() == tmp.ptr);
	CHECK(v.cast<Derived<1>*>() == tmp.ptr);
	CHECK(v.cast<Ptr>() == tmp);

	CHECK(!v.canCast<Derived<2>*>());
	CHECK(!v.canCast<DerivedPtr<2>>());
}

TEST(Variant_interchange)
{
	Variant v;
	CHECK(v.typeIs<void>());
	CHECK(v.isVoid());

	// to/from string conversion
	{
		Variant tmp = v.cast<std::string>();
		CHECK(areEqual(v, tmp));
		CHECK(areEqual(tmp, v));
	}

	// (de)serialization
	{
		ResizingMemoryStream dataStream;
		TextStream s(dataStream);
		s << v;
		CHECK(s.good());
		s.sync();

		CHECK_EQUAL("void", dataStream.buffer());

		Variant tmp;
		s.seek(0);
		s >> tmp;
		CHECK(!s.fail());

		CHECK(areEqual(v, tmp));
		CHECK(areEqual(tmp, v));
	}

	v = -1;
	variantCheck<int32_t>(EXTRA_ARGS, v, -1, "-1");

	v = (unsigned)1;
	variantCheck<uint32_t>(EXTRA_ARGS, v, 1, "1");

	v = -9223372036854775807;
	variantCheck<int64_t>(EXTRA_ARGS, v, -9223372036854775807, "-9223372036854775807");

	v = 10223372036854775807;
	variantCheck<uint64_t>(EXTRA_ARGS, v, 10223372036854775807, "10223372036854775807");

	v = (unsigned)1;
	variantCheck<uint32_t>(EXTRA_ARGS, v, 1, "1");

	v = 1.5f;
	variantCheck<double>(EXTRA_ARGS, v, 1.5f, "1.5");

	v = -1.6;
	variantCheck<double>(EXTRA_ARGS, v, -1.6, "-1.6");

	v = "qwe";
	variantCheck<std::string>(EXTRA_ARGS, v, "qwe", "\"qwe\"");

	Base s(13);
	v = s;
	variantCheck<Base>(EXTRA_ARGS, v, Base(13), "13 0.5 \"hello\"");

	Variant v1 = v;
	variantCheck<Base>(EXTRA_ARGS, v, Base(13), "13 0.5 \"hello\"");
	variantCheck<Base>(EXTRA_ARGS, v1, Base(13), "13 0.5 \"hello\"");

	v1 = Base(15);
	variantCheck<Base>(EXTRA_ARGS, v, Base(13), "13 0.5 \"hello\"");
	variantCheck<Base>(EXTRA_ARGS, v1, Base(15), "15 0.5 \"hello\"");
}

TEST(Variant_deserialize)
{
	// (u)int overflow -> double
	deserializeCheck<double>(EXTRA_ARGS, "18446744073709551616", 18446744073709551616.0);
	deserializeCheck<double>(EXTRA_ARGS, "-9223372036854775809", -9223372036854775809.0);

	// float e-notation
	deserializeCheck<double>(EXTRA_ARGS, "1.23e2", 123.0);
	deserializeCheck<double>(EXTRA_ARGS, "1.23e+2", 123.0);
	deserializeCheck<double>(EXTRA_ARGS, "-1.23e2", -123.0);
	deserializeCheck<double>(EXTRA_ARGS, "-1.23e+2", -123.0);

	deserializeCheck<double>(EXTRA_ARGS, "1.23e-2", 0.0123);
	deserializeCheck<double>(EXTRA_ARGS, "-1.23e-2", -0.0123);

	// hex
	deserializeCheck<uintmax_t>(EXTRA_ARGS, "0x0123abcd", 0x0123abcd);
	deserializeCheck<uintmax_t>(EXTRA_ARGS, "0x1234abcd", 0x1234abcd);

	// oct
	deserializeCheck<uintmax_t>(EXTRA_ARGS, "01234", 01234);

	// bin
	deserializeCheck<uintmax_t>(EXTRA_ARGS, "0b00011011", 0x1b);
}

TEST(Variant_with)
{
	int withCalls = 0;

	Variant v;
	CHECK(!v.visit<int>([](const int&) {}));

	v = 42;
	CHECK(v.visit<int>([&](const int& i) {
		CHECK_EQUAL(42, i);
		withCalls += 1;
	}));
	CHECK_EQUAL(1, withCalls);

	CHECK(v.visit<unsigned>([&](const unsigned& u) {
		CHECK_EQUAL(42, u);
		withCalls += 1;
	}));
	CHECK_EQUAL(2, withCalls);

	v = Base(8);
	CHECK(v.visit<Base>([&](const Base& b) {
		CHECK_EQUAL(8, b.i);
		withCalls += 1;
	}));
	CHECK_EQUAL(3, withCalls);

	CHECK(v.visit<Variant>([&](const Variant& t) {
		CHECK(&t == &v);
		CHECK(t == Base(8));
		withCalls += 1;
	}));
	CHECK_EQUAL(4, withCalls);
}

TEST(Variant_value_cast_ptr_and_ref)
{
	Variant v = Base(42);

	// matching pointer
	Base* pb = v.value<Base*>();
	RETURN_ON_FAIL_CHECK(pb != nullptr);
	CHECK_EQUAL(42, pb->i);
	pb->i = 1;
	CHECK(v == Base(1));

	// matching reference
	Base& rb = v.cast<Base&>();
	CHECK(&rb == pb);
	CHECK_EQUAL(1, rb.i);
	rb.i = 13;
	CHECK(v == Base(13));
}

TEST(Variant_ptr_cast_ptr_and_ref)
{
	Derived<42> d;
	DerivedPtr<42> dp(&d);
	Variant v = dp;

	// matching pointer
	Base* pb = v.cast<Base*>();
	CHECK(pb == &d);
	CHECK_EQUAL(42, pb->i);
	pb->i = 1;
	CHECK(v == Base(1));

	// matching reference
	Base& rb = v.cast<Base&>();
	CHECK(&rb == pb);
	CHECK_EQUAL(1, rb.i);
	rb.i = 13;
	CHECK(v == Base(13));
}
} // end namespace wgt
