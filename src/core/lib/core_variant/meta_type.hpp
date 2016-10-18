#ifndef META_TYPE_HPP
#define META_TYPE_HPP

#include "variant_dll.hpp"
#include "type_id.hpp"

#include "core_common/wg_dlink.hpp"

#include <type_traits>
#include <cstddef>


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

	template< typename T >
	struct StaticInstantiator
	{
		StaticInstantiator()
		{
			instance();
		}

		T& instance()
		{
			static T s_instance;
			return s_instance;
		}

		static StaticInstantiator< T > instantiator;
	};

	template< typename T >
	StaticInstantiator< T > StaticInstantiator< T >::instantiator;

}

template<typename T>
class MetaTypeImpl;

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
		Unqualified = 0,
		Const = 1,
		Volatile = 2
	};

	enum
	{
		QualifiersMask = Const | Volatile
	};

	template< typename T >
	struct qualifiers_of
	{
		static const int value =
			(std::is_const< T >::value ? Const : 0) |
			(std::is_volatile< T >::value ? Volatile : 0);
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
			if( !qualifiersMatch( srcQualifiers, destQualifiers ) )
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

	static bool qualifiersMatch(int src, int dest)
	{
		// dest must have all bits of src set
		return ( src & dest ) == src;
	}

	virtual ~MetaType();

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
	virtual bool convertFrom( void* to, const MetaType* fromType, const void* from ) const;
	bool canConvertFrom( const MetaType* toType ) const;

	/**
	Try convert a @a from value of this type to a @a to value of @a toType.
	*/
	bool convertTo( const MetaType* toType, void* to, const void* from ) const;
	bool canConvertTo( const MetaType* toType ) const;

	bool castPtr( const TypeId& destType, void** dest, void* src ) const;

	bool operator==( const MetaType& other ) const;

	bool operator!=( const MetaType& other ) const
	{
		return !( *this == other );
	}

	template<typename T>
	static const MetaType* get()
	{
		return &meta_type_details::StaticInstantiator<
			MetaTypeImpl<
				typename std::decay< T >::type
			>
		>::instantiator.instance();
	}

	static const MetaType* find(const char* name);
	static const MetaType* find(const TypeId& typeId);

protected:
	struct Data
	{
		const TypeId& typeId_;
		size_t size_;
		int flags_;
	};

	MetaType( const char* name, const Data& data );

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

private:
	Data data_;
	const char* name_; // allow custom (human readable) type name for MetaType
	Qualified qualified_[ QualifiersMask + 1 ];
	DLink link_;

	static void validateIndex();

};

} // end namespace wgt
#endif //META_TYPE_HPP
