#ifndef META_TYPE_HPP
#define META_TYPE_HPP

// define this to 1 once non-exception type cast is implemented
#define FAST_RUNTIME_POINTER_CAST 0

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

/**
Base metatype class for any type that can be used inside Variant.
*/
class MetaType
{
public:
	typedef bool (*ConversionFunc)( const MetaType* toType, void* to, const MetaType* fromType, const void* from );

	template< typename To, typename From >
	static bool straightConversion( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
	{
		To* t = reinterpret_cast< To* >( to );
		const From* f = reinterpret_cast< const From* >( from );

		*t = static_cast< To >( *f );

		return true;
	}

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
		DeducibleFromText = 2
	};

	MetaType(
		const char* name,
		size_t size,
		const TypeId& typeId,
		const std::type_info& typeInfo,
		const std::type_info* pointedType,
		int flags );

	virtual ~MetaType()
	{
	}

	const TypeId& typeId() const
	{
		return typeId_;
	}

	const char* name() const
	{
		return name_;
	}

	size_t size() const
	{
		return size_;
	}

	const std::type_info& typeInfo() const
	{
		return typeInfo_;
	}

	virtual void init(void* value) const = 0;
	virtual void copy(void* dest, const void* src) const = 0;
	virtual void move(void* dest, void* src) const = 0;
	virtual void destroy(void* value) const = 0;
	virtual bool equal(const void* lhs, const void* rhs) const = 0;

	const std::type_info* pointedType() const
	{
		return pointedType_;
	}

	int flags() const
	{
		return flags_;
	}

	bool testFlags( int test ) const
	{
		return ( flags_ & test ) == test;
	}

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

	template< typename T >
	T* castPtr( const void* value ) const
	{
		return castPtr< T >( value, true );
	}

	template< typename T >
	T* castPtr( void* value ) const
	{
		return castPtr< T >( value, false );
	}

#if !FAST_RUNTIME_POINTER_CAST

	/**
	Throw @a ptr casted the stored value type.

	`(T*)ptr` is thrown if stored type is either `T` or `T*`. I.e. if stored
	value is pointer then pointer itself is thrown, NOT pointer to
	pointer).
	*/
	virtual void throwPtr( void* ptr, bool const_value ) const = 0;

#endif // FAST_RUNTIME_POINTER_CAST

	bool operator == ( const MetaType& other ) const
	{
		return typeId_ == other.typeId_ && strcmp(name_, other.name_) == 0;
	}

protected:
	void addConversionFrom( const std::type_info& fromType, ConversionFunc func );

	template< typename From >
	void addConversionFrom( ConversionFunc func )
	{
		addConversionFrom( typeid( From ), func );
	}

	template< typename To, typename From >
	void addStraightConversion()
	{
		addConversionFrom( typeid( From ), &straightConversion< To, From > );
	}

	void setDefaultConversionFrom( ConversionFunc func );

private:
	struct TypeInfoHash
	{
		size_t operator()(const std::type_info* v) const
		{
			return v->hash_code();
		}
	};

	struct TypeInfosEq
	{
		bool operator()(const std::type_info* lhs, const std::type_info* rhs) const
		{
			return
				lhs == rhs ||
				*lhs == *rhs;
		}
	};

	const TypeId& typeId_;
	const char* name_; // allow custom (human readable) type name for MetaType
	size_t size_;
	const std::type_info& typeInfo_;
	const std::type_info* pointedType_;
	int flags_;

#if FAST_RUNTIME_POINTER_CAST

	// TODO: keep a list of all possible pointer conversions

#else // FAST_RUNTIME_POINTER_CAST

	struct PtrCastEntry
	{
		PtrCastEntry()
		{
			memset( this, 0, sizeof( *this ) );
		}

		bool tested[2];
		bool compatible[2];
		std::ptrdiff_t diff[2];
	};

	mutable std::mutex ptrCastsMutex_;
	mutable std::unordered_map< const std::type_info*, PtrCastEntry, TypeInfoHash, TypeInfosEq > ptrCasts_;

#endif // FAST_RUNTIME_POINTER_CAST

	std::unordered_map< const std::type_info*, ConversionFunc, TypeInfoHash, TypeInfosEq > conversionsFrom_;
	ConversionFunc defaultConversionFrom_;

#if FAST_RUNTIME_POINTER_CAST

	void* castPtr( const std::type_info& type, void* value, bool const_value ) const;

	template< typename T >
	T* castPtr( const void* value, bool const_value ) const
	{
		return castPtr( typeid( T ), ( void* )value, const_value );
	}

#else // FAST_RUNTIME_POINTER_CAST

	template< typename T >
	T* castPtr( const void* value, bool const_value ) const
	{
		char* initialPtr;
		if( pointedType_ )
		{
			initialPtr = *( char** )value;
			const_value = false; // pointer itself is const, but pointed value is probably non-const
		}
		else
		{
			initialPtr = ( char* )value;
		}

		if( !initialPtr )
		{
			return nullptr;
		}

		std::lock_guard< std::mutex > lock( ptrCastsMutex_ );
		auto& castEntry = ptrCasts_[ &typeid( T ) ];
		if( !castEntry.tested[ const_value ] )
		{
			// new entry
			try
			{
				throwPtr( initialPtr, const_value );
			}
			catch( T* e )
			{
				// conversion succeeded, record the difference
				castEntry.compatible[ const_value ] = true;
				castEntry.diff[ const_value ] = ( char* )e - initialPtr;
			}
			catch( ... )
			{
				// nop
			}

			castEntry.tested[ const_value ] = true;
		}

		if( castEntry.compatible[ const_value ] )
		{
			// apply diff to the pointer
			return ( T* )( initialPtr + castEntry.diff[ const_value ] );
		}
		else
		{
			// pointer types are incompatible
			return nullptr;
		}
	}

#endif // FAST_RUNTIME_POINTER_CAST

};
} // end namespace wgt
#endif //META_TYPE_HPP
