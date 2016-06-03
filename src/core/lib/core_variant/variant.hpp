#ifndef VARIANT_HPP_INCLUDED
#define VARIANT_HPP_INCLUDED

#include <string>
#include <typeinfo>
#include <iostream>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>

#include <cstdint>

#include "type_id.hpp"
#include "meta_type.hpp"
#include "interfaces/i_meta_type_manager.hpp"
#include <atomic>

#include "core_serialization/text_stream.hpp"
#include "core_serialization/binary_stream.hpp"

namespace wgt
{
class Variant;

/**
This namespace contains Variant type internal stuff.
*/
namespace variant_details
{

	/**
	Placeholder type used in templates below to select default behaviour.
	*/
	struct Default
	{
	};

	template< typename T1, typename T2 >
	struct is_same_no_cv:
		public std::is_same<
			typename std::remove_cv< T1 >::type,
			typename std::remove_cv< T2 >::type>
	{
	};

}

/**
Helper function used to reduce argument type to storage type.

This function (and its overloads) is used to get storage type for a bunch
of other compatible types. This allows to register one type and use it to
store a range of compatible types.

Argument type itself is used as storage by default.

You can define your own overloads for your custom types.
*/
variant_details::Default upcast(...);

/**
Helper function used to cast stored value to some other type.

Implicit cast is used by default.

You can define your own overloads for your custom types. If overload returns
other type than @c bool then this overload will never be called and
corresponding wil be rejected (at compile time).
*/
template<typename T, typename Storage>
bool downcast(T* v, const Storage& storage)
{
	if(v)
	{
		*v = storage;
	}
	return true;
}

/**
Helper function used to compare stored value with some value.

Default implementation performs comparison using operator==().

You can define your own overloads for your custom types.
*/
template<typename Storage, typename Value>
bool equal(const Storage& s, const Value& v)
{
	return s == v;
}

// uintmax_t

template<typename T>
typename std::enable_if<
	std::is_integral<T>::value &&
	std::is_unsigned<T>::value &&
	!variant_details::is_same_no_cv<T, bool>::value,
uintmax_t >::type upcast(T v)
{
	return v;
}

template<typename T>
bool downcast(T* v, uintmax_t storage)
{
	if(v)
	{
		*v = static_cast<T>(storage);
	}
	return true;
}

// intmax_t

template<typename T>
typename std::enable_if<
	std::is_integral<T>::value &&
	std::is_signed<T>::value &&
	!variant_details::is_same_no_cv<T, bool>::value,
intmax_t >::type upcast(T v)
{
	return v;
}

template<typename T>
bool downcast(T* v, intmax_t storage)
{
	if(v)
	{
		*v = static_cast<T>(storage);
	}
	return true;
}

// bool

/*
Use template function to avoid unexpected implicit conversion of different types
to bool.
*/
template<typename T>
typename std::enable_if< std::is_same<T, bool>::value, intmax_t >::type upcast(T v)
{
	return v;
}

inline bool downcast(bool* v, intmax_t storage)
{
	if(v)
	{
		*v = (storage != 0);
	}
	return true;
}

inline bool equal(intmax_t s, bool v)
{
	return (s != 0) == v;
}

// double

template< typename T >
typename std::enable_if< std::is_floating_point< T >::value,
double >::type upcast( T v )
{
	return static_cast< double >( v );
}

template< typename T >
bool downcast( T* v, double storage )
{
	if( v )
	{
		*v = static_cast< T >( storage );
	}
	return true;
}

// std::string

inline std::string upcast( std::string&& v )
{
	return std::move( v );
}

inline const std::string& upcast( const std::string & v )
{
	return v;
}

std::string upcast(const char* v);
std::string upcast(const std::wstring& v);
std::string upcast(const wchar_t* v);

bool downcast(std::wstring* v, const std::string& storage);

// forbid casting to raw string pointers as string lifetime is managed by Variant internals
void downcast(const char** v, const std::string& storage);
void downcast(const wchar_t** v, const std::string& storage);

inline bool equal(const std::string& s, const std::wstring& v)
{
	return (s == upcast( v ));
}

inline bool equal(const std::string& s, const wchar_t * v)
{
	return (s == upcast( v ));
}

// shared_ptr

/*template<typename T>
T* upcast(const std::shared_ptr<T>& v)
{
	return v.get();
}

// Deny downcasting to shared_ptr.
// When you reset shared_ptr by raw pointer a NEW reference counter is
// created. And if the same pointer is managed by two (or even more) shared
// counters you will eventually get multiple deallocations of the same
// object. So just don't do this.
//TODO: Look into std::enabled_shared_for_this
template<typename T>
void downcast(std::shared_ptr<T>* v, T* storage);*/

namespace variant_details
{

	namespace streaming
	{

		// hide variant streaming
		void operator<<( TextStream&, const Variant& );
		void operator>>( TextStream&, Variant& );
		void operator<<( BinaryStream&, const Variant& );
		void operator>>( BinaryStream&, Variant& );

		template<typename T>
		struct not_void
		{
			typedef typename std::decay< T >::type decayed_type;
			typedef typename std::enable_if< !std::is_same< decayed_type, void >::value >::type type;
		};

		struct Yes {};
		struct No {};

		template<typename Stream, typename T>
		static Yes checkStreamingOut(typename not_void<decltype(std::declval<Stream&>() << std::declval<const T&>())>::type*);

		template<typename Stream, typename T>
		static No checkStreamingOut(...);

		template<typename Stream, typename T>
		static Yes checkStreamingIn(typename not_void<decltype(std::declval<Stream&>() >> std::declval<T&>())>::type*);

		template<typename Stream, typename T>
		static No checkStreamingIn(...);

		// check operator<< and operator>> existence
		template<typename Stream, typename T>
		struct check
		{
			static const bool has_streaming_out = std::is_same<decltype(checkStreamingOut<Stream, T>(0)), Yes>::value;
			static const bool has_streaming_in = std::is_same<decltype(checkStreamingIn<Stream, T>(0)), Yes>::value;
		};

	};

	/**
	Modify type to allow its returning.

	This is almost the same as std::decay but it doesn't remove references
	and const/volatile when it's possible to keep them.
	*/
	template<typename T>
	struct Returnable
	{
		typedef typename std::remove_reference<T>::type no_ref;
		typedef typename std::conditional<
			std::is_array<no_ref>::value,
			typename std::remove_extent<no_ref>::type*,
			typename std::conditional<
				std::is_function<no_ref>::value,
				no_ref*,
				T
			>::type
		>::type type;
	};

	/**
	Implementation details of type traits used by Variant.
	*/
	template<typename T>
	struct TraitsImpl
	{
		typedef typename std::decay<T>::type value_type;

		/*
		Deduce upcast result type. If upcast returns Default then assume
		pass-through upcast (which just returns its argument).
		*/
		typedef decltype(upcast(std::declval<T>())) raw_direct_upcasted_type;
		typedef typename std::conditional<
			std::is_same<raw_direct_upcasted_type, Default>::value,
			typename Returnable<T>::type, // we can safely use reference here, as it's used as return type only of default (non-recursive) upcast
			raw_direct_upcasted_type
		>::type direct_upcasted_type;

		typedef typename std::decay<direct_upcasted_type>::type direct_storage_type; // storage type is deduced from upcast return value

		/**
		Helper struct used for resursive up- and downcast.
		Default version implements recursive cast and type deduction.

		Dummy is required to workaround restriction of specializations of
		nested template inside another template, see n3242 14.7.3/16.
		*/
		template<bool recur, typename Dummy = void>
		struct recursion_helper_impl
		{
			typedef TraitsImpl<direct_upcasted_type> recursive_traits;

			typedef typename recursive_traits::upcasted_type upcasted_type;
			typedef typename std::decay<upcasted_type>::type storage_type;
			typedef typename std::decay<decltype(downcast(std::declval<value_type*>(), std::declval<direct_storage_type>()))>::type direct_downcast_result_type;

			static const bool can_upcast = recursive_traits::can_upcast;
			static const bool can_downcast = recursive_traits::can_downcast;

			template<typename U>
			static upcasted_type upcast_helper(U&& v)
			{
				return recursive_traits::recursion_helper::upcast_helper(upcast(std::forward<U>(v)));
			}

			static bool downcast_helper(value_type* v, const storage_type* storage)
			{
				direct_storage_type tmp;
				if(!recursive_traits::recursion_helper::downcast_helper(&tmp, storage))
				{
					return false;
				}

				return downcast(v, tmp);
			}
		};

		/**
		Recursion-stopper specialization.
		*/
		template<typename Dummy>
		struct recursion_helper_impl<false, Dummy>
		{
			typedef direct_upcasted_type upcasted_type;
			typedef typename std::decay<upcasted_type>::type storage_type;
			typedef bool direct_downcast_result_type;

			static const bool can_upcast = true;
			static const bool can_downcast = true;

			template<typename U>
			static upcasted_type upcast_helper(U&& v)
			{
				return std::forward<U>(v);
			}

			static bool downcast_helper(value_type* v, const storage_type* storage)
			{
				if(v)
				{
					*v = *storage;
				}

				return true;
			}
		};

		static const bool upcasted_to_void = std::is_same<direct_storage_type, void>::value;
		static const bool recur =
			!upcasted_to_void &&
			!std::is_same<direct_storage_type, value_type>::value;

		typedef recursion_helper_impl<recur> recursion_helper;
		typedef typename recursion_helper::upcasted_type upcasted_type;
		typedef typename recursion_helper::storage_type storage_type;
		typedef typename recursion_helper::direct_downcast_result_type direct_downcast_result_type;

		static const bool can_upcast =
			!upcasted_to_void && // don't allow upcast if storage is void
			!std::is_convertible<value_type, const MetaType*>::value && // reject MetaType* (there's special constructor for it),
			!std::is_same<value_type, Variant>::value && // reject Variant type itself
			recursion_helper::can_upcast; // check whole upcast chain

		static const bool can_downcast =
			can_upcast && // if value can't be upcasted then corresponding downcast doesn't make any sense
			std::is_same<direct_downcast_result_type, bool>::value &&
			recursion_helper::can_downcast; // check whole downcast chain

		static const bool has_text_streaming_out = streaming::check<TextStream, T>::has_streaming_out;
		static const bool has_text_streaming_in = streaming::check<TextStream, T>::has_streaming_in;
		static const bool has_binary_streaming_out = streaming::check<BinaryStream, T>::has_streaming_out;
		static const bool has_binary_streaming_in = streaming::check<BinaryStream, T>::has_streaming_in;

	};

	/**
	Helper struct that provides recursive downcast implementation in case
	the whole downcast chain is valid.
	*/
	template<typename T, bool can_downcast>
	class Downcaster
	{
		typedef TraitsImpl<T> traits_impl;

	public:
		static bool downcast(typename traits_impl::value_type* v, const typename traits_impl::storage_type& storage)
		{
			return traits_impl::recursion_helper::downcast_helper(v, &storage);
		}
	};

	template<typename T>
	class Downcaster<T, false>
	{
	};

	/**
	Helper struct that provides recursive upcast implementation in case
	the whole upcast chain is valid.
	*/
	template<typename T, bool can_upcast>
	class Upcaster
	{
		typedef TraitsImpl<T> traits_impl;

	public:
		template<typename U>
		static typename traits_impl::upcasted_type upcast(U&& v)
		{
			return traits_impl::recursion_helper::upcast_helper(std::forward<U>(v));
		}

		typedef T upcastable_type;

	};

	template<typename T>
	class Upcaster<T, false>
	{
	};

	/**
	Helper struct that provides access to either existing text-streaming-out
	implementation or error-stub.
	*/
	template<typename T, bool has_text_streaming_out>
	struct TextStreamerOut
	{
		static void streamOut( TextStream& stream, const T& value )
		{
			stream << value;
		}
	};

	template<typename T>
	struct TextStreamerOut<T, false>
	{
		static void streamOut( TextStream& stream, const T& )
		{
			stream.setState( std::ios_base::failbit );
		}
	};

	/**
	Helper struct that provides access to either existing text-streaming-in
	implementation or error-stub.
	*/
	template<typename T, bool has_text_streaming_in>
	struct TextStreamerIn
	{
		static void streamIn( TextStream& stream, T& value )
		{
			stream >> value;
		}
	};

	template<typename T>
	struct TextStreamerIn<T, false>
	{
		static void streamIn( TextStream& stream, T& )
		{
			stream.setState( std::ios_base::failbit );
		}
	};

	/**
	Helper struct that provides access to either existing binary-streaming-out
	implementation or error-stub.
	*/
	template<typename T, bool has_binary_streaming_out>
	struct BinaryStreamerOut
	{
		static void streamOut( BinaryStream& stream, const T& value )
		{
			stream << value;
		}
	};

	template<typename T>
	struct BinaryStreamerOut<T, false>
	{
		static void streamOut( BinaryStream& stream, const T& )
		{
			stream.setState( std::ios_base::failbit );
		}
	};

	/**
	Helper struct that provides access to either existing binary-streaming-in
	implementation or error-stub.
	*/
	template<typename T, bool has_binary_streaming_in>
	struct BinaryStreamerIn
	{
		static void streamIn( BinaryStream& stream, T& value )
		{
			stream >> value;
		}
	};

	template<typename T>
	struct BinaryStreamerIn<T, false>
	{
		static void streamIn( BinaryStream& stream, T& )
		{
			stream.setState( std::ios_base::failbit );
		}
	};

	/**
	Traits for pointer types.
	*/
	template< typename T, bool is_ptr >
	struct PointerTraitsImpl
	{
		static const bool is_pointer = false;
		typedef T throw_type;

		static const std::type_info* pointedType()
		{
			return nullptr;
		}
	};

	template< typename T >
	struct PointerTraitsImpl< T*, true >
	{
		static const bool is_pointer = true;
		typedef T throw_type;

		static const std::type_info* pointedType()
		{
			return &typeid( T );
		}
	};

	template< typename T >
	struct PointerTraits:
		public PointerTraitsImpl< T, std::is_pointer< T >::value >
	{
	};

}

class Variant
{
	friend TextStream& operator<<( TextStream& stream, const Variant& value );
	friend TextStream& operator>>( TextStream& stream, Variant& value );
	friend BinaryStream& operator<<( BinaryStream& stream, const Variant& value );
	friend BinaryStream& operator>>( BinaryStream& stream, Variant& value );

public:
	/**
	Helper structure used to get useful information about types Variant works with.
	*/
	template<typename T>
	struct traits:
		public variant_details::Upcaster<T, variant_details::TraitsImpl<T>::can_upcast>,
		public variant_details::Downcaster<T, variant_details::TraitsImpl<T>::can_downcast>,
		public variant_details::TextStreamerOut<T, variant_details::TraitsImpl<T>::has_text_streaming_out>,
		public variant_details::TextStreamerIn<T, variant_details::TraitsImpl<T>::has_text_streaming_in>,
		public variant_details::BinaryStreamerOut<T, variant_details::TraitsImpl<T>::has_binary_streaming_out>,
		public variant_details::BinaryStreamerIn<T, variant_details::TraitsImpl<T>::has_binary_streaming_in>,
		public variant_details::PointerTraits<T>
	{
		typedef variant_details::TraitsImpl<T> traits_impl;

	public:
		using variant_details::TextStreamerOut<T, variant_details::TraitsImpl<T>::has_text_streaming_out>::streamOut;
		using variant_details::TextStreamerIn<T, variant_details::TraitsImpl<T>::has_text_streaming_in>::streamIn;
		using variant_details::BinaryStreamerOut<T, variant_details::TraitsImpl<T>::has_binary_streaming_out>::streamOut;
		using variant_details::BinaryStreamerIn<T, variant_details::TraitsImpl<T>::has_binary_streaming_in>::streamIn;

		typedef typename traits_impl::value_type value_type;
		typedef typename traits_impl::upcasted_type upcasted_type;
		typedef typename traits_impl::storage_type storage_type;

		static const bool can_upcast = traits_impl::can_upcast;
		static const bool can_downcast = traits_impl::can_downcast;
	};

	/**
	Construct @c void (i.e. empty) variant.
	*/
	Variant();

	/**
	Construct a copy.
	Variant copy has the same type and value as another one.
	*/
	Variant(const Variant& value);

	/**
	Move variant value.
	*/
	Variant(Variant&& value);

	/**
	Construct variant of specified type with default value.
	*/
	explicit Variant(const MetaType* type);

	/**
	Construct variant by conversion of @a value to @a type.

	If conversion fails then @c std::bad_cast is thrown.

	@see convert
	*/
	Variant(const MetaType* type, const Variant& value);

	/**
	Construct variant from a given value.

	If value type is not registered then either @c std::bad_cast is thrown (if
	@a voidOnFail is `false`) of `void` Variant is constructed (if @a voidOnFail
	if `true`).

	@see registerType
	*/
	template<typename T>
	Variant(T&& value, typename std::enable_if<traits<T>::can_upcast, bool>::type voidOnFail = false)
	{
		if(!tryInit(std::forward<T>(value)))
		{
			if(voidOnFail)
			{
				initVoid();
			}
			else
			{
				typeInitError();
			}
		}
	}

	/**
	Destroy underlying value and free all resources used by this Variant.
	*/
	~Variant()
	{
		destroy();
	}

	/**
	Assign another variant value to this one.
	*/
	Variant& operator=(const Variant& value);

	/**
	Move another variant value to this one.
	*/
	Variant& operator=(Variant&& value);

	/**
	Assign given value to the Variant.
	If value type is not registered then @c std::bad_cast is thrown.

	@see registerType
	*/
	template< typename T >
	typename std::enable_if< traits< T >::can_upcast, Variant& >::type operator=( T&& value )
	{
		if( typeIs< T >() )
		{
			detach( false );
			assign( std::forward< T >( value ) );
		}
		else
		{
			destroy();
			if( !tryInit( std::forward< T >( value ) ) )
			{
				typeInitError();
			}
		}

		return *this;
	}

	/**
	Compare two variants.

	Built-in types (and all supported conversions) may be cross-compared,
	i.e. @c int may be compared with @a double.

	User types (i.e. registered outside) may be compared only with each
	other, their native support (i.e. comparison operator overloads) of
	comparing with other types doesn't matter. Different user types will
	never match.

	Also any type may be compared with string. Comparison succeeds if string
	deserialization to a corresponding type succeeds and resulting values match.
	*/
	bool operator==(const Variant& v) const;

	bool operator!=(const Variant& v) const
	{
		return !(*this == v);
	}

	/**
	Compare variant with arbitrary value.

	Built-in and user types are supported.
	*/
	template<typename T>
	typename std::enable_if<traits<T>::can_upcast, bool>::type operator==(const T& value) const
	{
		typedef typename traits<T>::storage_type storage_type;

		bool eq = false;
		with<storage_type>([&](const storage_type& v)
		{
			eq = equal(v, value);
		});

		return eq;
	}

	template<typename T>
	typename std::enable_if<traits<T>::can_upcast, bool>::type operator!=(const T& value) const
	{
		return !(*this == value);
	}

	/**
	Type of currently stored value.
	*/
	const MetaType* type() const
	{
		return type_;
	}

	/**
	Try to convert current value to the given @a type.

	@return @c true if conversion succeeds, @c false otherwise.
	*/
	bool convert(const MetaType* type);

	/**
	Convenience overload.
	*/
	template<typename T>
	bool convert()
	{
		typedef typename traits<T>::storage_type storage_type;
		return convert(findType<storage_type>());
	}

	/**
	Check if variant has @c void type (i.e. it has no value).
	*/
	bool isVoid() const;

	/**
	Check if variant contains value of pointer type.
	Any pointer type can be cast to <tt>void*</tt> type.
	*/
	bool isPointer() const;

	/**
	Check if current value has exact type.
	*/
	template<typename T>
	bool typeIs() const
	{
		auto type = findType<T>();
		return type_ == type || (type != nullptr && *type_ == *type);
	}

	/**
	Try to cast current value to the given type pointer.

	This cast relates more to an upcast in terms of standard pointer conversion,
	i.e. cast to base, cast to qualified pointer, etc.

	If cast fails then `nullptr` is returned.
	*/
	template< typename T >
	T* castPtr() const
	{
		return type_->castPtr< T >( payload() );
	}

	/**
	@overload
	*/
	template< typename T >
	T* castPtr()
	{
		if( !std::is_const< T >::value && !isPointer() )
		{
			detach( true );
		}

		return type_->castPtr< T >( payload() );
	}

	/**
	Try to cast current value to the given type reference.

	If cast fails then @c std::bad_cast is thrown.
	*/
	template< typename T >
	T& castRef() const
	{
		auto result = castPtr< T >();
		if( !result )
		{
			castError();
		}

		return *result;
	}

	/**
	@overload
	*/
	template< typename T >
	T& castRef()
	{
		auto result = castPtr< T >();
		if( !result )
		{
			castError();
		}

		return *result;
	}

	/**
	Check if current value may be casted to the given type.
	*/
	template< typename T >
	typename std::enable_if<traits<T>::can_downcast, bool>::type canCast() const
	{
		return tryCastImpl( ( T* ) nullptr );
	}

	/**
	Try to cast current value to the given type.

	Returns @c true if cast succeeded, @c false otherwise.

	Output value can be changed even after unsuccessful cast.
	*/
	template< typename T >
	typename std::enable_if< traits< T >::can_downcast, bool >::type tryCast( T& out ) const
	{
		return tryCastImpl( &out );
	}

	/**
	Try to cast current value to the given type.

	If cast fails then @c std::bad_cast is thrown.
	*/
	template< typename T >
	typename std::enable_if< traits< T >::can_downcast, T >::type cast() const
	{
		T result;
		if( !tryCastImpl(&result) )
		{
			castError();
		}

		return result;
	}

	/**
	Try to cast current value to the given type.
	If cast fails then value constructed by default is returned.
	*/
	template< typename T >
	typename std::enable_if< traits< T >::can_downcast, T >::type value() const
	{
		T result;

		if( !tryCastImpl(&result) )
		{
			result = T();
		}

		return result;
	}

	/**
	Try to cast current value to the given type.
	If cast fails then the given default value is returned.
	*/
	template< typename T >
	typename std::enable_if< traits< T >::can_downcast, T >::type value( const T& def ) const
	{
		T result;

		if( !tryCastImpl(&result) )
		{
			result = def;
		}

		return result;
	}

	/**
	Call given function with current value casted to the given type.

	This function allows to avoid value copy when possible.

	@return @c true if cast was succeeded and the function was called, @c false if
	cast was failed and the function was not called.
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		std::is_same< T, Variant >::value,
		bool >::type with( const Fn& fn ) const
	{
		fn( *this );
		return true;
	}

	/**
	@overload
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		traits< T >::can_downcast,
		bool >::type with( const Fn& fn ) const
	{
		if( auto ptr = castPtr< const T >() )
		{
			fn( *ptr );
			return true;
		}

		T tmp;
		if( tryCastValImpl( &tmp ) )
		{
			fn( tmp );
			return true;
		}

		return false;
	}

	/**
	@overload
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		!std::is_same< T, Variant >::value && !traits< T >::can_downcast,
		bool >::type with( const Fn& fn ) const
	{
		if( auto ptr = castPtr< const T >() )
		{
			fn( *ptr );
			return true;
		}

		return false;
	}

	// Must be used before any other function on Variant.
	static void setMetaTypeManager( IMetaTypeManager * metaTypeManager );
	static IMetaTypeManager * getMetaTypeManager();

	/**
	Register type in current metatype manager (if any).
	*/
	static bool registerType(const MetaType* type);

	/**
	Templated version of findType.

	@see IMetaTypeManager::findType
	*/
	template<typename T>
	static const MetaType* findType()
	{
		return findTypeImpl<typename std::decay<T>::type>();
	}

	static const MetaType* findType( const TypeId& typeId )
	{
		return getMetaTypeManager()->findType( typeId );
	}

	static const MetaType* findType( const std::type_info& typeInfo )
	{
		return getMetaTypeManager()->findType( typeInfo );
	}

	static const MetaType* findType( const char* name )
	{
		return getMetaTypeManager()->findType( name );
	}

	/**
	Check whether given type is registered.

	@see findType
	*/
	template<typename T>
	static bool typeIsRegistered()
	{
		return findType<T>() != nullptr;
	}

private:
	static const size_t INLINE_PAYLOAD_SIZE = sizeof( std::shared_ptr< void > );

	class DynamicData
	{
	public:
		template<typename T>
		static DynamicData* allocate()
		{
			return allocate(sizeof(T));
		}

		static DynamicData* allocate(size_t payloadSize);

		void incRef()
		{
			refs_.fetch_add( 1 );
		}

		void decRef(const MetaType* type);

		/**
		Check if there's only one reference to this data.
		*/
		bool isExclusive() const
		{
			return refs_ == 0;
		}

		void* payload()
		{
			return this + 1;
		}

	private:
		DynamicData():
			refs_( 0 )
		{
		}

		std::atomic< int > refs_;
	};

	/**
	The way data is stored depends on payload (i.e. stored type) size.
	If payload size fits into INLINE_PAYLOAD_SIZE then it's stored in inline
	payload. Otherwise dynamically allocated storage is used.

	Dynamically allocated storage is shared among all variant copies.
	*/
	union Data
	{
		char payload_[INLINE_PAYLOAD_SIZE];
		DynamicData* dynamic_;
	};

	const MetaType* type_;
	Data data_;

	bool isInline() const
	{
		return
			type_->size() <= INLINE_PAYLOAD_SIZE &&
			!type_->testFlags( MetaType::ForceShared );
	}

	void* payload()
	{
		if( isInline() )
		{
			return data_.payload_;
		}
		else
		{
			return data_.dynamic_->payload();
		}
	}

	const void* payload() const
	{
		return const_cast< Variant* >( this )->payload();
	}

	template<typename T>
	const T& forceCast() const
	{
		return *reinterpret_cast<const T*>(payload());
	}

	template<typename T>
	T& forceCast()
	{
		return *reinterpret_cast<T*>(payload());
	}

	template<typename T>
	static const MetaType* findTypeImpl()
	{
		return getMetaTypeManager()->findType(TypeId::getType<T>());
	}

	template< typename T >
	bool tryInit( T&& value )
	{
		typedef typename traits< T >::storage_type storage_type;

		type_ = findType< storage_type >();
		if( type_ == nullptr )
		{
			return false;
		}

		void* p;
		if( isInline() )
		{
			p = data_.payload_;
		}
		else
		{
			data_.dynamic_ = DynamicData::allocate< storage_type >();
			p = data_.dynamic_->payload();
		}

		new (p) storage_type( traits< T >::upcast( std::forward< T >( value ) ) );
		return true;
	}

	void initVoid();
	void init(const Variant& value);
	void init(Variant&& value);

	template<typename T>
	typename std::enable_if<std::is_rvalue_reference<T>::value>::type assign(T&& value)
	{
		type_->move(payload(), &value);
	}

	template<typename T>
	typename std::enable_if<!std::is_rvalue_reference<T>::value>::type assign(const T& value)
	{
		type_->copy(payload(), &value);
	}

	/**
	Cast using custom conversions and Variant-specific downcasting.
	*/
	template< typename T >
	bool tryCastValImpl( T* out ) const
	{
		typedef typename traits< T >::value_type value_type;
		typedef typename traits< T >::storage_type storage_type;

		const MetaType* storageType = findType< storage_type >();
		if( !storageType )
		{
			return false;
		}

		if( type_ == storageType )
		{
			return traits< T >::downcast( out, forceCast< storage_type >() );
		}

		if( std::is_same< value_type, storage_type >::value )
		{
			if( out )
			{
				return type_->convertTo( storageType, out, payload() );
			}
			else
			{
				storage_type tmp;
				return type_->convertTo( storageType, &tmp, payload() );
			}
		}
		else
		{
			storage_type tmp;
			if( type_->convertTo( storageType, &tmp, payload() ) )
			{
				return traits< T >::downcast( out, tmp );
			}
		}

		return false;
	}

	/**
	Perform standard pointer cast and then custom casts.
	*/
	template< typename T >
	typename std::enable_if< !traits< T >::is_pointer, bool >::type tryCastImpl( T* out ) const
	{
		if( auto ptr = castPtr< T >() )
		{
			if( out )
			{
				*out = *ptr;
			}
			return true;
		}

		return tryCastValImpl( out );
	}

	template< typename T >
	typename std::enable_if< traits< T* >::is_pointer, bool >::type tryCastImpl( T** out ) const
	{
		if( auto ptr = castPtr< T >() )
		{
			if( out )
			{
				*out = ptr;
			}
			return true;
		}

		return tryCastValImpl( out );
	}

	void destroy();
	void detach( bool copy );

	static void castError();
	static void typeInitError();

};


/**
Serialize Variant to a text stream.
*/
TextStream& operator<<( TextStream& stream, const Variant& value );

/**
Deserialize Variant from a text stream.

Variant type may be given explicitly or deduced implicitly. Only these basic
types may be deduced: void, signed/unsigned integer, real, string. If neither
explicit type was given (input value has void type) nor type can be deduced
then deserialization fails.
*/
TextStream& operator>>( TextStream& stream, Variant& value );

/**
Serialize Variant to a binary stream.
*/
BinaryStream& operator<<( BinaryStream& stream, const Variant& value );

/**
Deserialize Variant from a binary stream.

Variant type must be given explicitly.
*/
BinaryStream& operator>>( BinaryStream& stream, Variant& value );

/**
Text streaming wrapper for std::ostream.
*/
std::ostream& operator<<( std::ostream& stream, const Variant& value );

/**
Text streaming wrapper for std::istream.
*/
std::istream& operator>>( std::istream& stream, Variant& value );

template<typename T>
class MetaTypeImplNoStream:
	public MetaType
{
	typedef MetaType base;
	typedef T value_type;
	typedef typename Variant::traits< value_type > traits;

public:
	MetaTypeImplNoStream( const char* name, int flags ):
		base(
			name,
			sizeof( value_type ),
			TypeId::getType< value_type >(),
			typeid( value_type ),
			traits::pointedType(),
			flags )
	{
	}

	void init(void* value) const override
	{
		new (value) value_type();
	}

	void copy(void* dest, const void* src) const override
	{
		cast(dest) = cast(src);
	}

	void move(void* dest, void* src) const override
	{
		cast(dest) = std::move(cast(src));
	}

	void destroy(void* value) const override
	{
		cast(value).~value_type();
	}

	bool equal(const void* lhs, const void* rhs) const override
	{
		return cast(lhs) == cast(rhs);
	}

#if !FAST_RUNTIME_POINTER_CAST

	void throwPtr( void* ptr, bool const_value ) const override
	{
		if( const_value )
		{
			throw ( const typename traits::throw_type* )ptr;
		}
		else
		{
			throw ( typename traits::throw_type* )ptr;
		}
	}

#endif

protected:
	static value_type& cast(void* value)
	{
		return *static_cast<value_type*>(value);
	}

	static const value_type& cast(const void* value)
	{
		return *static_cast<const value_type*>(value);
	}

};


/**
Default implementation of MetaType.
*/
template<typename T>
class MetaTypeImpl:
	public MetaTypeImplNoStream<T>
{
	typedef MetaTypeImplNoStream<T> base;
	typedef T value_type;

public:
	explicit MetaTypeImpl(const char* name = nullptr, int flags = 0):
		base( name, flags )
	{
	}

	void streamOut(TextStream& stream, const void* value) const override
	{
		Variant::traits<value_type>::streamOut(stream, base::cast(value));
	}

	void streamIn(TextStream& stream, void* value) const override
	{
		Variant::traits<value_type>::streamIn(stream, base::cast(value));
	}

	void streamOut(BinaryStream& stream, const void* value) const override
	{
		Variant::traits<value_type>::streamOut(stream, base::cast(value));
	}

	void streamIn(BinaryStream& stream, void* value) const override
	{
		Variant::traits<value_type>::streamIn(stream, base::cast(value));
	}

};
} // end namespace wgt
#endif // VARIANT_HPP_INCLUDED
