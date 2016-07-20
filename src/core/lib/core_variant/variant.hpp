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
#include <cassert>

#include "type_id.hpp"
#include "meta_type.hpp"
#include <atomic>

#include "core_serialization/text_stream.hpp"
#include "core_serialization/binary_stream.hpp"

#include "variant_dll.hpp"

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
corresponding downcast will be rejected (at compile time).
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
typename std::enable_if<
	std::is_same<T, bool>::value,
	intmax_t >::type upcast(T v)
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

// double

template< typename T >
typename std::enable_if<
	std::is_floating_point< T >::value,
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

std::string VARIANT_DLL upcast(const char* v);
std::string VARIANT_DLL upcast(const std::wstring& v);
std::string VARIANT_DLL upcast(const wchar_t* v);

bool VARIANT_DLL downcast(std::wstring* v, const std::string& storage);

// forbid casting to raw string pointers as string lifetime is managed by Variant internals
void downcast(const char** v, const std::string& storage);
void downcast(const wchar_t** v, const std::string& storage);

// nullptr_t

inline void* upcast(std::nullptr_t)
{
	return nullptr;
}

void downcast(std::nullptr_t* v, void* storage);


namespace variant_details
{

	namespace streaming
	{

		// hide variant streaming
		void operator<<( TextStream&, const Variant& );
		void operator>>( TextStream&, Variant& );
		void operator<<( BinaryStream&, const Variant& );
		void operator>>( BinaryStream&, Variant& );

		template< typename T >
			struct not_void_impl
		{
			typedef T type;
		};

		template<>
		struct not_void_impl< void >
		{
		};

		template< typename T >
		struct not_void:
			not_void_impl< typename std::decay< T >::type >
		{
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

	template<typename T>
	struct is_variant:
		is_same_no_cv< T, Variant >
	{
	};

	template< typename T >
	struct is_shared_ptr_impl:
		std::false_type
	{
	};

	template< typename T1 >
	struct is_shared_ptr_impl< std::shared_ptr< T1 > >:
		std::true_type
	{
	};

	template< typename T >
	struct is_shared_ptr:
		is_shared_ptr_impl< typename std::remove_cv< T >::type >
	{
	};


	/**
	Modify type to allow its returning.

	This is almost the same as std::decay but it doesn't remove references (when
	possible) and keeps const/volatile.
	*/
	template< typename T >
	struct Returnable
	{
		typedef typename std::remove_reference< T >::type no_ref;
		typedef typename std::conditional<
			std::is_array< no_ref >::value,
			typename std::remove_extent< no_ref >::type*,
			typename std::conditional<
				std::is_function< no_ref >::value,
				no_ref*,
				T
			>::type
		>::type type;
	};

	/**
	Modify type to allow its storing.

	This is almost the same as std::decay but it keeps const/volatile.
	*/
	template< typename T >
	struct Storable
	{
		typedef typename std::remove_reference< T >::type no_ref;
		typedef typename std::conditional<
			std::is_array< no_ref >::value,
			typename std::remove_extent< no_ref >::type*,
			typename std::conditional<
				std::is_function< no_ref >::value,
				no_ref*,
				no_ref
			>::type
		>::type type;
	};

#if _MSC_VER == 1700

	/**
	Workaround MSVC 2012 bug which causes compilation error in case of

		std::pointer_traits< std::shared_ptr< SomeAbstractClass > >

	See https://connect.microsoft.com/VisualStudio/feedback/details/781594
	*/
	template< typename T >
	struct my_pointer_traits
	{
		typedef typename T::element_type element_type;
	};

	template< typename T >
	struct my_pointer_traits< T* >
	{
		typedef T element_type;
	};

#else

	template< typename T >
	struct my_pointer_traits:
		std::pointer_traits< T >
	{
	};

#endif

	template< typename T, bool is_pointer >
	struct safe_element_type
	{
		// use decay to support types like `T* const&`
		typedef typename my_pointer_traits< typename std::decay< T >::type >::element_type type;
	};

	template< typename T >
	struct safe_element_type< T, false >
	{
		typedef void type; // this case should never be used
	};

	/**
	Implementation details of type traits used by Variant.
	*/
	template<typename T>
	struct TraitsImpl
	{
		typedef typename Storable<T>::type value_type;

		/*
		Deduce upcast result type. If upcast returns Default then assume
		pass-through upcast (which just returns its argument).
		*/
		typedef decltype(upcast(std::declval<T>())) raw_direct_upcasted_type;
		typedef typename std::conditional<
			std::is_same<raw_direct_upcasted_type, Default>::value,
			typename Returnable<T>::type,
			raw_direct_upcasted_type
		>::type direct_upcasted_type;

		typedef typename Storable<direct_upcasted_type>::type direct_storage_type; // storage type is deduced from upcast return value

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
			typedef decltype(downcast(std::declval<value_type*>(), std::declval<direct_storage_type>())) direct_downcast_result_type;

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
				return downcast(v, *storage);
			}
		};

		static const bool upcasted_to_void = std::is_same<direct_storage_type, void>::value;
		static const bool pass_through = std::is_same<direct_storage_type, value_type>::value;
		static const bool recur = !upcasted_to_void && !pass_through;

		typedef recursion_helper_impl<recur> recursion_helper;
		typedef typename recursion_helper::upcasted_type upcasted_type;
		typedef typename recursion_helper::storage_type storage_type;
		typedef typename recursion_helper::direct_downcast_result_type direct_downcast_result_type;

		static const bool is_variant = variant_details::is_variant<value_type>::value;
		static const bool is_meta_type =
			std::is_convertible<value_type, const MetaType*>::value &&
			!variant_details::is_same_no_cv<value_type, std::nullptr_t>::value;

		static const bool can_upcast =
			!upcasted_to_void && // don't allow upcast if storage is void
			!is_meta_type && // reject MetaType* (there's special constructor for it),
			!is_variant && // reject Variant type itself
			recursion_helper::can_upcast; // check whole upcast chain

		static const bool can_downcast =
			can_upcast && // if value can't be upcasted then corresponding downcast doesn't make any sense
			std::is_same<direct_downcast_result_type, bool>::value &&
			recursion_helper::can_downcast; // check whole downcast chain

		static const bool can_cast = can_downcast || is_variant;

		static const bool has_text_streaming_out = streaming::check<TextStream, T>::has_streaming_out;
		static const bool has_text_streaming_in = streaming::check<TextStream, T>::has_streaming_in;
		static const bool has_binary_streaming_out = streaming::check<BinaryStream, T>::has_streaming_out;
		static const bool has_binary_streaming_in = streaming::check<BinaryStream, T>::has_streaming_in;

		static const bool raw_ptr_storage = std::is_pointer<storage_type>::value;
		static const bool shared_ptr_storage = is_shared_ptr<storage_type>::value;
		static const bool value_storage = !raw_ptr_storage && !shared_ptr_storage;

		typedef typename safe_element_type<
			storage_type,
			raw_ptr_storage || shared_ptr_storage
		>::type storage_element_type;
	};

	/**
	Helper struct that provides recursive downcast implementation in case
	the whole downcast chain is valid.
	*/
	template<typename T, bool can_downcast = TraitsImpl<T>::can_downcast>
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
	template<typename T, bool can_upcast = TraitsImpl<T>::can_upcast>
	class Upcaster
	{
		typedef TraitsImpl<T> traits_impl;

	public:
		template<typename U>
		static typename traits_impl::upcasted_type upcast(U&& v)
		{
			return traits_impl::recursion_helper::upcast_helper(std::forward<U>(v));
		}
	};

	template<typename T>
	class Upcaster<T, false>
	{
	};

	/**
	Helper struct that provides access to either existing text-streaming-out
	implementation or error-stub.
	*/
	template<typename T, bool has_text_streaming_out = TraitsImpl<T>::has_text_streaming_out>
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
	template<typename T, bool has_text_streaming_in = TraitsImpl<T>::has_text_streaming_in>
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
	template<typename T, bool has_binary_streaming_out = TraitsImpl<T>::has_binary_streaming_out>
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
	template<typename T, bool has_binary_streaming_in = TraitsImpl<T>::has_binary_streaming_in>
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
	Helper struct to workaround compiler warnings for bool vs. int comparison.
	*/
	template< typename T >
	struct EqualityCompare
	{
		template< typename U >
		bool operator()( const U& storage, const T& value ) const
		{
			return storage == value;
		}
	};

	template<>
	struct EqualityCompare< bool >
	{
		template< typename U >
		bool operator()( const U& storage, bool value ) const
		{
			return ( storage != 0 ) == value;
		}
	};

}

}

/*
Due to Argument Dependent Lookup up- and downcast for shared_ptr must be
declared in `std` namespace. Unfortunately.
*/
namespace std
{

	// shared_ptr

	/*
	Default shared_ptr up- and downcasting use corresponding raw pointer up- and
	downcasting, so you're generally required to declare only the latter.
	*/
	template< typename T >
	typename enable_if<
		wgt::variant_details::TraitsImpl< T* >::can_upcast &&
		wgt::variant_details::TraitsImpl< T* >::raw_ptr_storage,
		shared_ptr< typename wgt::variant_details::TraitsImpl< T* >::storage_element_type >
	>::type upcast( const shared_ptr< T >& v )
	{
		typedef typename wgt::variant_details::TraitsImpl< T* >::storage_element_type storage_element_type;
		storage_element_type* ptr = wgt::variant_details::Upcaster< T* >::upcast( v.get() );
		return shared_ptr< storage_element_type >( v, ptr );
	}

	template< typename T >
	typename enable_if<
		wgt::variant_details::TraitsImpl< T* >::can_downcast &&
		wgt::variant_details::TraitsImpl< T* >::raw_ptr_storage,
		bool
	>::type downcast(
		shared_ptr< T >* v,
		const shared_ptr< typename wgt::variant_details::TraitsImpl< T* >::storage_element_type >& storage )
	{
		T* ptr = nullptr;
		if( !wgt::variant_details::Downcaster< T* >::downcast( &ptr, storage.get() ) )
		{
			return false;
		}

		if( v )
		{
			*v = shared_ptr< T >( storage, ptr );
		}
		return true;
	}

	// unique_ptr

	/*
	You can move unique_ptr to Variant, but you can't get unique_ptr back.
	*/
	template< typename T, typename D >
	shared_ptr< T > upcast( unique_ptr< T, D >&& v )
	{
		return shared_ptr< T >( std::move( v ) );
	}

	template< typename T, typename D >
	void downcast( unique_ptr< T, D >* v, const shared_ptr< T >& storage );

}

namespace wgt
{

class VARIANT_DLL Variant
{
public:
	/**
	Helper structure used to get useful information about types Variant works with.
	*/
	template< typename T >
	struct traits:
		public variant_details::Upcaster< T >,
		public variant_details::Downcaster< T >,
		public variant_details::TextStreamerOut< T >,
		public variant_details::TextStreamerIn< T >,
		public variant_details::BinaryStreamerOut< T >,
		public variant_details::BinaryStreamerIn< T >
	{
		typedef variant_details::TraitsImpl< T > traits_impl;

	public:
		using variant_details::TextStreamerOut< T >::streamOut;
		using variant_details::TextStreamerIn< T >::streamIn;
		using variant_details::BinaryStreamerOut< T >::streamOut;
		using variant_details::BinaryStreamerIn< T >::streamIn;

		typedef typename traits_impl::value_type value_type;
		typedef typename traits_impl::upcasted_type upcasted_type;
		typedef typename traits_impl::storage_type storage_type;
		typedef typename traits_impl::storage_element_type storage_element_type;

		static const bool is_variant = traits_impl::is_variant;

		static const bool pass_through = traits_impl::pass_through;
		static const bool can_upcast = traits_impl::can_upcast;
		static const bool can_downcast = traits_impl::can_downcast;
		static const bool can_cast = traits_impl::can_cast;

		static const bool raw_ptr_storage = traits_impl::raw_ptr_storage;
		static const bool shared_ptr_storage = traits_impl::shared_ptr_storage;
		static const bool value_storage = traits_impl::value_storage;
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

	@see convert, setType
	*/
	Variant(const MetaType* type, const Variant& value);

	/**
	Construct variant by conversion of @a value to @a type.

	If conversion fails then Variant is set to @c void. If @a succeeded is not
	@c nullptr then it takes @c true in case of success, and @c false otherwise.

	@see convert, setType
	*/
	Variant(const MetaType* type, const Variant& value, bool* succeeded);

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
	typename std::enable_if<
		traits< T >::can_upcast,
		Variant& >::type operator=( T&& that )
	{
		typedef typename traits< T >::storage_type storage_type;

		if( auto ptr = value< storage_type* >() )
		{
			*ptr = traits< T >::upcast( std::forward< T >( that ) );
		}
		else
		{
			destroy();
			if( !tryInit( std::forward< T >( that ) ) )
			{
				typeInitError();
			}
		}

		return *this;
	}

	/**
	Compare two variants.

	All types (and all supported conversions) may be cross-compared,
	i.e. @c int may be compared with @a double.
	*/
	bool operator==(const Variant& that) const;

	bool operator!=(const Variant& that) const
	{
		return !(*this == that);
	}

	/**
	Compare variant with arbitrary value.
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::can_downcast,
		bool >::type operator==( const T& that ) const
	{
		typedef typename traits< T >::storage_type storage_type;

		bool eq = false;
		visit< storage_type >( [&]( const storage_type& v )
		{
			eq = variant_details::EqualityCompare< typename std::decay< const T& >::type >()( v, that );
		} );

		return eq;
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_downcast,
		bool >::type operator!=( const T& that ) const
	{
		return !( *this == that );
	}

	/**
	Type of currently stored value.
	*/
	const MetaType* type() const
	{
		return qualifiedType()->type();
	}

	/**
	Try to convert current value to the given @a type.

	@return @c true if conversion succeeds, @c false otherwise.
	*/
	bool setType( const MetaType* type );

	/**
	@overload.
	*/
	template< typename T >
	bool setType()
	{
		typedef typename traits< T >::storage_type storage_type;
		return setType( findType< storage_type >() );
	}

	/**
	Try to convert current value to the given @a type.

	@return conversion result or void Variant if failed.
	*/
	Variant convert( const MetaType* type, bool* succeeded = nullptr ) const
	{
		return Variant( type, *this, succeeded );
	}

	/**
	@overload.
	*/
	template< typename T >
	Variant convert( bool* succeeded = nullptr ) const
	{
		typedef typename traits< T >::storage_type storage_type;
		return convert( findType< storage_type >(), succeeded );
	}

	/**
	Check if variant has @c void type (i.e. it has no value).

	Note that void pointer is not void type, so this function returns @c false
	in this case.
	*/
	bool isVoid() const;

	/**
	Check if variant contains value of pointer type.

	Any pointer type can be cast to <tt>void*</tt> type (with regard for
	qualifiers).
	*/
	bool isPointer() const;

	/**
	Check if variant contains null pointer (either raw or shared) of any type.
	*/
	bool isNullPointer() const;

	/**
	Check if current value has exact type.
	*/
	template<typename T>
	bool typeIs() const
	{
		return type()->typeId() == TypeId::getType< T >();
	}

	/**
	Check if current value may be casted to the given type.
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		!std::is_reference< T >::value,
		bool >::type canCast() const
	{
		return tryCastImpl( ( T* ) nullptr );
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		!std::is_reference< T >::value,
		bool >::type canCast()
	{
		return tryCastImpl( ( T* ) nullptr );
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		std::is_reference< T >::value,
		bool >::type canCast() const
	{
		typedef typename std::remove_reference< T >::type ref_target;
		return tryCastImpl( ( ref_target** ) nullptr );
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		std::is_reference< T >::value,
		bool >::type canCast()
	{
		typedef typename std::remove_reference< T >::type ref_target;
		return tryCastImpl( ( ref_target** ) nullptr );
	}

	/**
	Try to cast current value to the given type.

	Returns @c true if cast succeeded, @c false otherwise.

	Output value can be changed even after unsuccessful cast.
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast,
		bool >::type tryCast( T& dest ) const
	{
		return tryCastImpl( &dest );
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast,
		bool >::type tryCast( T& dest )
	{
		return tryCastImpl( &dest );
	}

	/**
	Try to cast current value to the given type.

	If cast fails then @c std::bad_cast is thrown.
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		!std::is_reference< T >::value,
		T >::type cast() const
	{
		T result;
		if( !tryCast( result ) )
		{
			castError();
		}

		return result;
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		!std::is_reference< T >::value,
		T >::type cast()
	{
		T result;
		if( !tryCast( result ) )
		{
			castError();
		}

		return result;
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		std::is_reference< T >::value,
		T >::type cast() const
	{
		typedef typename std::remove_reference< T >::type ref_target;
		ref_target* result;

		if( !tryCast( result ) )
		{
			castError();
		}

		return *result;
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast &&
		std::is_reference< T >::value,
		T >::type cast()
	{
		typedef typename std::remove_reference< T >::type ref_target;
		ref_target* result;

		if( !tryCast( result ) )
		{
			castError();
		}

		return *result;
	}

	/**
	Try to cast current value to the given type.
	If cast fails then value constructed by default is returned.
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast,
		typename traits< T >::value_type >::type value() const
	{
		typedef typename traits< T >::value_type value_type;
		value_type result;

		if( !tryCast( result ) )
		{
			result = value_type();
		}

		return result;
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast,
		typename traits< T >::value_type >::type value()
	{
		typedef typename traits< T >::value_type value_type;
		value_type result;

		if( !tryCast( result ) )
		{
			result = value_type();
		}

		return result;
	}

	/**
	Try to cast current value to the given type.
	If cast fails then the given default value is returned.
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast,
		typename traits< T >::value_type >::type value( const T& def ) const
	{
		typedef typename traits< T >::value_type value_type;
		value_type result;

		if( !tryCast( result ) )
		{
			result = value_type();
		}

		return result;
	}

	template< typename T >
	typename std::enable_if<
		traits< T >::can_cast,
		typename traits< T >::value_type >::type value( const T& def )
	{
		typedef typename traits< T >::value_type value_type;
		value_type result;

		if( !tryCast( result ) )
		{
			result = value_type();
		}

		return result;
	}

	/**
	Call given function with current value casted to the given type.

	This function allows to avoid value copy when possible.

	@return @c true if the cast was succeeded and the function was called,
	@c false if the cast was failed and the function was not called.

	@internal
	Variant
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		traits< T >::is_variant,
		bool >::type visit( const Fn& fn ) const
	{
		fn( *this );
		return true;
	}

	/**
	@internal
	generic value without downcast
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		traits< T >::can_downcast &&
		traits< T >::pass_through &&
		traits< T >::value_storage,
		bool >::type visit( const Fn& fn ) const
	{
		typedef typename traits< T >::storage_type storage_type;

		const storage_type* ptr = nullptr;
		if( tryCast( ptr ) )
		{
			if( ptr )
			{
				fn( *ptr );
				return true;
			}
		}
		else if( auto destStorageType = findType< storage_type >() )
		{
			storage_type tmp;
			if( type()->convertTo( destStorageType, &tmp, value< const void* >() ) )
			{
				fn( tmp );
				return true;
			}
		}

		return false;
	}

	/**
	@internal
	generic raw pointer without downcast
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		traits< T >::can_downcast &&
		traits< T >::pass_through &&
		traits< T >::raw_ptr_storage,
		bool >::type visit( const Fn& fn ) const
	{
		typedef typename traits< T >::storage_element_type storage_element_type;

		storage_element_type* ptr = nullptr;
		if( tryCast( ptr ) )
		{
			// ptr may be null
			fn( ptr );
			return true;
		}

		return false;
	}

	/**
	@internal
	generic shared pointer without downcast
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		traits< T >::can_downcast &&
		traits< T >::pass_through &&
		traits< T >::shared_ptr_storage,
		bool >::type visit( const Fn& fn ) const
	{
		typedef typename traits< T >::storage_type storage_type;

		storage_type ptr;
		if( tryCast( ptr ) )
		{
			// ptr may be null (i.e. empty)
			fn( ptr );
			return true;
		}

		return false;
	}

	/**
	@internal
	generic with downcast
	*/
	template< typename T, typename Fn >
	typename std::enable_if<
		traits< T >::can_downcast &&
		!traits< T >::pass_through,
		bool >::type visit( const Fn& fn ) const
	{
		typedef typename traits< T >::value_type value_type;
		typedef typename traits< T >::storage_type storage_type;

		bool r = false;

		visit< storage_type >( [&]( const storage_type& storage )
		{
			value_type tmp;
			if( traits< T >::downcast( &tmp, storage ) )
			{
				fn( tmp );
				r = true;
			}
		} );

		return r;
	}

	/**
	Register type in current metatype manager (if any).
	*/
	static bool registerType(const MetaType* type);

	/**
	Templated version of findType.
	*/
	template<typename T>
	static const MetaType* findType()
	{
		return findType(TypeId::getType<typename std::decay<T>::type>());
	}

	static const MetaType* findType( const TypeId& typeId );
	static const MetaType* findType( const std::type_info& typeInfo );
	static const MetaType* findType( const char* name );

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
	static const uintptr_t STORAGE_KIND_MASK = 0x03;

	enum StorageKind
	{
		Inline = 0, // value is held inline
		RawPointer = 1, // value is passed as raw pointer, ownership belongs to user
		SharedPointer = 2, // value is held in shared_ptr
		COW = 3 // value is held as pointer to dynamic storage with copy-on-write access
	};

	class VARIANT_DLL COWData
	{
	public:
		template<typename T>
		static COWData* allocate()
		{
			return allocate(sizeof(T));
		}

		static COWData* allocate(size_t payloadSize);

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

		const void* payload() const
		{
			return this + 1;
		}

	private:
		COWData():
			refs_( 0 )
		{
		}

		std::atomic< int > refs_;
	};

	union Data
	{
		char inline_[INLINE_PAYLOAD_SIZE];
		void* rawPointer_;
		COWData* cow_;
		char sharedPointer_[sizeof(std::shared_ptr<void>)];

		const std::shared_ptr<void>& sharedPointer() const
		{
			return reinterpret_cast< const std::shared_ptr<void>& >( sharedPointer_ );
		}

		std::shared_ptr<void>& sharedPointer()
		{
			return reinterpret_cast< std::shared_ptr<void>& >( sharedPointer_ );
		}

		COWData* cow()
		{
			return cow_;
		}

		const COWData* cow() const
		{
			return cow_;
		}
	};

	const void* type_;
	Data data_;

	StorageKind storageKind() const
	{
		return static_cast< StorageKind >(
			reinterpret_cast< uintptr_t >( type_ ) &
			STORAGE_KIND_MASK );
	}

	const MetaType::Qualified* qualifiedType() const
	{
		return reinterpret_cast< const MetaType::Qualified* >(
			reinterpret_cast< uintptr_t >( type_ ) &
			~STORAGE_KIND_MASK );
	}

	void setTypeInternal( const MetaType::Qualified* t, StorageKind s )
	{
		assert( ( reinterpret_cast< uintptr_t >( t ) & STORAGE_KIND_MASK ) == 0 );
		type_ = reinterpret_cast< const MetaType* >(
			reinterpret_cast< uintptr_t >( t ) | (
				static_cast< uintptr_t >( s ) &
				STORAGE_KIND_MASK ) );
	}

	static bool isInline( const MetaType* type )
	{
		return
			type->size() <= INLINE_PAYLOAD_SIZE &&
			!type->testFlags( MetaType::ForceShared );
	}

	static const MetaType::Qualified* findQualifiedType( const TypeId& typeId, int qualifiers )
	{
		if( auto type = findType( typeId ) )
		{
			return type->qualified( qualifiers );
		}

		return nullptr;
	}

	template< typename T >
	static const MetaType::Qualified* findQualifiedType()
	{
		if( auto type = findType< T >() )
		{
			return type->qualified( MetaType::qualifiers_of< T >::value );
		}

		return nullptr;
	}

	template< typename T >
	typename std::enable_if<
		traits<T>::value_storage,
		bool >::type tryInit( T&& value )
	{
		typedef typename traits< T >::storage_type storage_type;

		auto type = findQualifiedType< storage_type >();
		if( type == nullptr )
		{
			return false;
		}

		void* p;
		if( isInline( type->type() ) )
		{
			setTypeInternal( type, Inline );
			p = data_.inline_;
		}
		else
		{
			setTypeInternal( type, COW ); 
			data_.cow_ = COWData::allocate< storage_type >();
			p = data_.cow()->payload();
		}

		new (p) storage_type( traits< T >::upcast( std::forward< T >( value ) ) );

		return true;
	}

	template< typename T >
	typename std::enable_if<
		traits<T>::raw_ptr_storage,
		bool >::type tryInit( T&& value )
	{
		typedef typename traits< T >::storage_element_type storage_element_type;

		auto type = findQualifiedType< storage_element_type >();
		if( type == nullptr )
		{
			return false;
		}

		setTypeInternal( type, RawPointer ); 
		// strip const-qualifier, as it's checked at runtime
		data_.rawPointer_ = (void*)traits< T >::upcast( std::forward< T >( value ) );

		return true;
	}

	template< typename T >
	typename std::enable_if<
		traits<T>::shared_ptr_storage,
		bool >::type tryInit( T&& value )
	{
		typedef typename traits< T >::storage_element_type storage_element_type;

		auto type = findQualifiedType< storage_element_type >();
		if( type == nullptr )
		{
			return false;
		}

		setTypeInternal( type, SharedPointer );
		new (data_.sharedPointer_) std::shared_ptr< void >(
			traits< T >::upcast( std::forward< T >( value ) ) );

		return true;
	}

	void initVoid();
	void init(const Variant& value);
	void init(Variant&& value);

	bool convertInit( const MetaType* type, const Variant& value );

	/**
	Try to cast current value to a given type.

	First, pointer cast is performed. If it succeeds then resulting pointer is
	downcasted to an @a dest.

	If pointer cast failed then conversion is performed, and in case of success
	result of conversion is downcasted to an @a dest.

	Note that @a dest can be null.

	@internal
	generic value without downcast
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::value_storage &&
		!traits< T >::is_variant &&
		traits< T >::pass_through,
		bool >::type tryCastImpl( T* dest ) const
	{
		typedef typename traits< T >::storage_type storage_type;

		const storage_type* ptr = nullptr;
		if( tryCast( ptr ) )
		{
			if( ptr )
			{
				return traits< T >::downcast( dest, *ptr );
			}
		}
		else if( auto destStorageType = findType< storage_type >() )
		{
			if( dest )
			{
				return type()->convertTo( destStorageType, dest, value< const void* >() );
			}
			else
			{
				storage_type tmp;
				return type()->convertTo( destStorageType, &tmp, value< const void* >() );
			}
		}

		return false;
	}

	/**
	@internal
	generic value with downcast
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::value_storage &&
		!traits< T >::is_variant &&
		!traits< T >::pass_through,
		bool >::type tryCastImpl( T* dest ) const
	{
		bool r = false;

		typedef typename traits< T >::storage_type storage_type;
		visit< storage_type >( [&]( const storage_type& storage )
		{
			r = traits< T >::downcast( dest, storage );
		} );

		return r;
	}

	/**
	@internal
	Variant value
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::is_variant,
		bool >::type tryCastImpl( T* dest ) const
	{
		if( dest )
		{
			*dest = *this;
		}
		return true;
	}

	/**
	@internal
	shared_ptr value
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::shared_ptr_storage,
		bool >::type tryCastImpl( T* dest ) const
	{
		if( storageKind() != SharedPointer )
		{
			return false;
		}

		typedef typename traits< T >::storage_element_type storage_element_type;

		storage_element_type* ptr = nullptr;
		if( !tryCast( ptr ) )
		{
			return false;
		}

		std::shared_ptr< storage_element_type > tmp( data_.sharedPointer(), ptr );
		return traits< T >::downcast( dest, tmp );
	}

	/**
	@internal
	generic pointer const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		!variant_details::is_variant< typename traits< T >::storage_element_type >::value &&
		!std::is_void< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest ) const
	{
		typename traits< T >::storage_element_type* ptr = nullptr;

		auto t = qualifiedType();
		switch( storageKind() )
		{
		case Inline:
			if( !t->castPtr( &ptr, data_.inline_ ) )
			{
				return false;
			}
			break;

		case RawPointer:
			if( !t->castPtr( &ptr, data_.rawPointer_ ) )
			{
				return false;
			}
			break;

		case SharedPointer:
			if( !t->castPtr( &ptr, data_.sharedPointer().get() ) )
			{
				return false;
			}
			break;

		case COW:
			if( !t->castPtr( &ptr, data_.cow()->payload() ) )
			{
				return false;
			}
			break;

		default:
			assert( false );
			return false;

		}

		return traits< T >::downcast( dest, ptr );
	}

	/**
	@internal
	generic pointer non-const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		!variant_details::is_variant< typename traits< T >::storage_element_type >::value &&
		!std::is_void< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest )
	{
		typename traits< T >::storage_element_type* ptr = nullptr;

		auto t = qualifiedType();
		switch( storageKind() )
		{
		case Inline:
			if( !t->castPtr( &ptr, data_.inline_ ) )
			{
				return false;
			}
			break;

		case RawPointer:
			if( !t->castPtr( &ptr, data_.rawPointer_ ) )
			{
				return false;
			}
			break;

		case SharedPointer:
			if( !t->castPtr( &ptr, data_.sharedPointer().get() ) )
			{
				return false;
			}
			break;

		case COW:
			if( !std::is_const< T >::value )
			{
				detach( true );
			}

			if( !t->castPtr( &ptr, data_.cow()->payload() ) )
			{
				return false;
			}
			break;

		default:
			assert( false );
			return false;

		}

		return traits< T >::downcast( dest, ptr );
	}

	/**
	@internal
	const Variant pointer const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		variant_details::is_variant< typename traits< T >::storage_element_type >::value &&
		std::is_const< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest ) const
	{
		return traits< T >::downcast( dest, this );
	}

	/**
	@internal
	Variant pointer non-const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		variant_details::is_variant< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest )
	{
		return traits< T >::downcast( dest, this );
	}

	/**
	@internal
	const void pointer const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		std::is_void< typename traits< T >::storage_element_type >::value &&
		std::is_const< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest ) const
	{
		typename traits< T >::storage_element_type* ptr = nullptr;
		switch( storageKind() )
		{
		case Inline:
			ptr = data_.inline_;
			break;

		case RawPointer:
			ptr = data_.rawPointer_;
			break;

		case SharedPointer:
			ptr = data_.sharedPointer().get();
			break;

		case COW:
			ptr = data_.cow()->payload();
			break;

		default:
			assert( false );
			return false;

		}

		return traits< T >::downcast( dest, ptr );
	}

	/**
	@internal
	non-const void pointer const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		std::is_void< typename traits< T >::storage_element_type >::value &&
		!std::is_const< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest ) const
	{
		typename traits< T >::storage_element_type* ptr = nullptr;
		switch( storageKind() )
		{
		case Inline:
			return false;

		case RawPointer:
			if( type()->testFlags( MetaType::Const ) )
			{
				return false;
			}

			ptr = data_.rawPointer_;
			break;

		case SharedPointer:
			if( type()->testFlags( MetaType::Const ) )
			{
				return false;
			}

			ptr = data_.sharedPointer().get();
			break;

		case COW:
			return false;

		default:
			assert( false );
			return false;

		}

		return traits< T >::downcast( dest, ptr );
	}

	/**
	@internal
	void pointer non-const function
	*/
	template< typename T >
	typename std::enable_if<
		traits< T >::raw_ptr_storage &&
		std::is_void< typename traits< T >::storage_element_type >::value,
		bool >::type tryCastImpl( T* dest )
	{
		typedef typename traits< T >::storage_element_type storage_element_type;
		if( !std::is_const< storage_element_type >::value && qualifiedType()->testQualifiers( MetaType::Const ) )
		{
			return false;
		}

		storage_element_type* ptr = nullptr;
		switch( storageKind() )
		{
		case Inline:
			ptr = data_.inline_;
			break;

		case RawPointer:
			ptr = data_.rawPointer_;
			break;

		case SharedPointer:
			ptr = data_.sharedPointer().get();
			break;

		case COW:
			if( !std::is_const< storage_element_type >::value )
			{
				detach( true );
			}
			ptr = data_.cow()->payload();
			break;

		default:
			assert( false );
			return false;

		}

		return traits< T >::downcast( dest, ptr );
	}

	void destroy();
	void detach( bool copy );

	static void castError();
	static void typeInitError();

};

/**
Serialize Variant to a text stream.
*/
VARIANT_DLL TextStream& operator<<( TextStream& stream, const Variant& value );

/**
Deserialize Variant from a text stream.

Variant type may be given explicitly or deduced implicitly. Only these basic
types may be deduced: void, signed/unsigned integer, real, string. If neither
explicit type was given (input value has void type) nor type can be deduced
then deserialization fails.
*/
VARIANT_DLL TextStream& operator>>( TextStream& stream, Variant& value );

/**
Serialize Variant to a binary stream.
*/
VARIANT_DLL BinaryStream& operator<<( BinaryStream& stream, const Variant& value );

/**
Deserialize Variant from a binary stream.

Variant type must be given explicitly.
*/
VARIANT_DLL BinaryStream& operator>>( BinaryStream& stream, Variant& value );

/**
Text streaming wrapper for std::ostream.
*/
VARIANT_DLL std::ostream& operator<<( std::ostream& stream, const Variant& value );

/**
Text streaming wrapper for std::istream.
*/
VARIANT_DLL std::istream& operator>>( std::istream& stream, Variant& value );

template<typename T>
class MetaTypeImplNoStream:
	public MetaType
{
	typedef MetaType base;
	typedef T value_type;
	typedef typename Variant::traits< value_type > traits;

public:
	MetaTypeImplNoStream( const char* name, int flags ):
		base( name, data< T >( flags ) )
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
