#ifndef GLOBAL_TYPE_ID_HPP
#define GLOBAL_TYPE_ID_HPP

#include <stdint.h>
#include <functional>
#include <typeinfo>
#include <string>
#include "variant_dll.hpp"

namespace wgt
{
class VARIANT_DLL TypeId
{
private:
	TypeId( const std::string & name );

public:
	TypeId( const char * name = nullptr );
	TypeId( const char * name, uint64_t hashCode );
	TypeId( const TypeId & other );

	~TypeId();

	uint64_t getHashcode() const { return hashCode_; }
	const char * getName() const { return name_; }

	bool isPointer() const;
	TypeId removePointer() const;

private:
	bool removePointer( TypeId * typeId ) const;

public:
	TypeId & operator = ( const TypeId & other );

	bool operator == ( const TypeId & other ) const;
	bool operator != ( const TypeId & other ) const;
	bool operator < (const TypeId & other) const;

	//==========================================================================
	template< typename T >
	static const TypeId& getType()
	{
		static const TypeId s_typeId( typeid( T ).name() );
		return s_typeId;
	}

private:
	const char *	name_;
	uint64_t		hashCode_;
	bool			owns_;
};
} // end namespace wgt

namespace std
{
	template<>
	struct hash< wgt::TypeId >
		: public unary_function< wgt::TypeId, size_t >
	{
	public:
		size_t operator()( const wgt::TypeId & v ) const
		{
			hash< uint64_t > hash_fn;
			return hash_fn( v.getHashcode() );
		}
	};

	template<>
	struct hash< const wgt::TypeId >
		: public unary_function< const wgt::TypeId, size_t >
	{
	public:
		size_t operator()( const wgt::TypeId & v ) const
		{
			hash< uint64_t > hash_fn;
			return hash_fn( v.getHashcode() );
		}
	};
}

#endif //GLOBAL_TYPE_ID_HPP
