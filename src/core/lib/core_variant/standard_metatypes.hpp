#ifndef STANDARD_METATYPES_HPP_INCLUDED
#define STANDARD_METATYPES_HPP_INCLUDED

#include "variant.hpp"
#include "variant_dll.hpp"

namespace wgt
{
// void

template <>
class VARIANT_DLL MetaTypeImpl<void> : public DefaultMetaTypeImplBase<void>
{
	typedef DefaultMetaTypeImplBase<void> base;

public:
	MetaTypeImpl();

	void init(void* value) const override;
	void copy(void* dest, const void* src) const override;
	void move(void* dest, void* src) const override;
	void destroy(void* value) const override;
	bool lessThan(const void* lhs, const void* rhs) const override;
	bool equal(const void* lhs, const void* rhs) const override;

	void streamOut(TextStream& stream, const void* value) const override;
	void streamIn(TextStream& stream, void* value) const override;
	void streamOut(BinaryStream& stream, const void* value) const override;
	void streamIn(BinaryStream& stream, void* value) const override;

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
	uint64_t hashCode(const void * value) const override;

};

template <>
class VARIANT_DLL MetaTypeImpl<uint32_t> : public DefaultMetaTypeImpl<uint32_t>
{
	typedef DefaultMetaTypeImpl<uint32_t> base;

public:
	MetaTypeImpl();

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
};

template <>
class VARIANT_DLL MetaTypeImpl<int32_t> : public DefaultMetaTypeImpl<int32_t>
{
	typedef DefaultMetaTypeImpl<int32_t> base;

public:
	MetaTypeImpl();

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
};

template <>
class VARIANT_DLL MetaTypeImpl<uint64_t> : public DefaultMetaTypeImpl<uint64_t>
{
	typedef DefaultMetaTypeImpl<uint64_t> base;

public:
	MetaTypeImpl();

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
};

template <>
class VARIANT_DLL MetaTypeImpl<int64_t> : public DefaultMetaTypeImpl<int64_t>
{
	typedef DefaultMetaTypeImpl<int64_t> base;

public:
	MetaTypeImpl();

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
};

template <>
class VARIANT_DLL MetaTypeImpl<double> : public DefaultMetaTypeImpl<double>
{
	typedef DefaultMetaTypeImpl<double> base;

public:
	MetaTypeImpl();

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
};

template <>
class VARIANT_DLL MetaTypeImpl<std::string> : public DefaultMetaTypeImplNoStream<std::string>
{
	typedef DefaultMetaTypeImplNoStream<std::string> base;

public:
	MetaTypeImpl();

	void streamOut(TextStream& stream, const void* value) const override;
	void streamIn(TextStream& stream, void* value) const override;
	void streamOut(BinaryStream& stream, const void* value) const override;
	void streamIn(BinaryStream& stream, void* value) const override;

	bool convertFrom(void* to, const MetaType* fromType, const void* from) const override;
};
}

#endif
