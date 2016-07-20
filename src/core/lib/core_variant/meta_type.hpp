#ifndef META_TYPE_HPP
#define META_TYPE_HPP

// define this to 1 once non-exception type cast is implemented
#define FAST_RUNTIME_POINTER_CAST 0

#include "variant_dll.hpp"

#include "type_id.hpp"
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <limits>
#include <mutex>
#include <cstdint>
#include <cstring>

namespace wgt
{
class TextStream;
class BinaryStream;

namespace meta_type_details
{

	template< typename T, bool is_void >
	struct size_traits_impl
	{
		static const size_t size = sizeof( T );
	};

	template< typename T >
	struct size_traits_impl< T, true >
	{
		static const size_t size = 0;
	};

	template< typename T >
	struct size_traits:
		size_traits_impl< T, std::is_void< T >::value >
	{
	};
}

/**
Base metatype class for any type that can be used inside Variant.
*/

class VARIANT_DLL MetaType
{
public:
	enum Flag
	{
		/**
		Prefer dynamic storage and implicit sharing (shallow copy). Set this
		flag for types that are expensive to copy.
		*/
		ForceShared = 1,

		/**
		This type can be deduced from a textual representation. Currently only
		some basic types are deducible.
		*/
		DeducibleFromText = 2,
	};

	enum Qualifier
	{
		Const = 1,
		Volatile = 2,
		QualifiersMask = Const | Volatile
	};

	template< typename T >
	struct qualifiers_of
	{
		static const int value =
			std::is_const< T >::value ? Const : 0 |
			std::is_volatile< T >::value ? Volatile : 0;
	};

	class Qualified
	{
		friend class MetaType;

	public:
		const MetaType* type() const
		{
			return type_;
		}

		int qualifiers() const
		{
			return static_cast< int >( this - type_->qualified_ );
		}

		bool testQualifiers( int test ) const
		{
			return ( qualifiers() & test ) == test;
		}

		template< typename Dest, typename Src >
		bool castPtr( Dest** dest, Src* src ) const
		{
			// check qualifiers
			auto srcQualifiers = qualifiers_of< Src >::value | qualifiers();
			auto destQualifiers = qualifiers_of< Dest >::value;
			if( ( srcQualifiers & ~destQualifiers ) != 0 )
			{
				// conversion loses qualifiers, fail
				return false;
			}

			// check types
			return type()->castPtr(
				TypeId::getType< Dest >(),
				(void**)dest,
				(void*)src );
		}

	private:
		const MetaType* type_;

	};

	virtual ~MetaType()
	{
	}

	const Qualified* qualified( int qualifiers ) const;

	const TypeId& typeId() const
	{
		return data_.typeId_;
	}

	const char* name() const
	{
		return name_;
	}

	size_t size() const
	{
		return data_.size_;
	}

	int flags() const
	{
		return data_.flags_;
	}

	bool testFlags( int test ) const
	{
		return ( data_.flags_ & test ) == test;
	}

	virtual void init(void* value) const = 0;
	virtual void copy(void* dest, const void* src) const = 0;
	virtual void move(void* dest, void* src) const = 0;
	virtual void destroy(void* value) const = 0;
	virtual bool equal(const void* lhs, const void* rhs) const = 0;

	virtual void streamOut( TextStream& stream, const void* value ) const = 0;
	virtual void streamIn( TextStream& stream, void* value ) const = 0;
	
	virtual void streamOut( BinaryStream& stream, const void* value ) const = 0;
	virtual void streamIn( BinaryStream& stream, void* value ) const = 0;

	/**
	Try convert a @a from value of @a fromType to a @a to value of this type.
	*/
	bool convertFrom( void* to, const MetaType* fromType, const void* from ) const;
	bool canConvertFrom( const MetaType* toType ) const;

	/**
	Try convert a @a from value of this type to a @a to value of @a toType.
	*/
	bool convertTo( const MetaType* toType, void* to, const void* from ) const;
	bool canConvertTo( const MetaType* toType ) const;

	bool castPtr( const TypeId& destType, void** dest, void* src ) const;

	bool operator==( const MetaType& other ) const
	{
		return
			data_.typeId_ == other.data_.typeId_ &&
			strcmp( name_, other.name_ ) == 0;
	}

protected:
	struct Data
	{
		const TypeId& typeId_;
		size_t size_;
		int flags_;
	};

	typedef bool (*ConversionFunc)( const MetaType* toType, void* to, const MetaType* fromType, const void* from );

	template< typename To, typename From >
	static bool straightConversion( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
	{
		To* t = reinterpret_cast< To* >( to );
		const From* f = reinterpret_cast< const From* >( from );

		*t = static_cast< To >( *f );

		return true;
	}

	template< typename T >
	static Data data( int flags = 0 )
	{
		Data result =
		{
			TypeId::getType< T >(),
			meta_type_details::size_traits< T >::size,
			flags
		};
		return result;
	}

	MetaType( const char* name, const Data& data );

	void addConversionFrom( const TypeId& fromType, ConversionFunc func );

	template< typename From >
	void addConversionFrom( ConversionFunc func )
	{
		addConversionFrom( TypeId::getType< From >(), func );
	}

	template< typename To, typename From >
	void addStraightConversion()
	{
		addConversionFrom( TypeId::getType< From >(), &straightConversion< To, From > );
	}

	void setDefaultConversionFrom( ConversionFunc func );

private:
	Data data_;
	const char* name_; // allow custom (human readable) type name for MetaType
	Qualified qualified_[ QualifiersMask + 1 ];

	std::unordered_map< TypeId, ConversionFunc > conversionsFrom_;
	ConversionFunc defaultConversionFrom_;

};

} // end namespace wgt
#endif //META_TYPE_HPP
