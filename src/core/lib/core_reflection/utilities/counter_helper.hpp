#ifndef COUNTER_HELPER_HPP
#define COUNTER_HELPER_HPP

//==============================================================================
// This helper generates an incrementing compile time counter used for
// auto registration of reflection definitions
// It might be possible to generalize this for other uses but that is an 
// exercise I will leave to the next user.
// The one thing to remember is that the counter only works within a single TU
// If you need to use this in multiple TUs, it would be best to scope them 
// in either the anonymous namespace or unique namespace.
//==============================================================================
namespace wgt
{

namespace ReflectionAutoRegistration
{
	template< size_t N >
	struct IdentifierT
	{
		//Declare a friend function for argument dependent lookup that we will
		//define elsewhere
		friend constexpr size_t friendFunc(IdentifierT< N >);
		static constexpr size_t value = N;
	};

	//When we instantiate InstantiateFriend< IdentifierT< N > >, we will end up
	//defining the friend function friendFunc( IdentifierT< N > )
	template< typename Identifier>
	struct InstantiateFriendFunc
	{
		//Define the friend function for argument dependent lookup
		friend constexpr size_t friendFunc(Identifier)
		{
			return Identifier::value;
		}
		static constexpr size_t value = Identifier::value;
	};

	//This will always be evaluated first because 0 doesn't need to be converted
	//So whether this is chosen depends on the the second template argument
	//If adl_lookup( IdentifierT< N > has not been defined yet, the compiler
	//will eval the following implementation of countReader due to SFINAE
	template< size_t N, int = friendFunc(IdentifierT< N > {}) >
	static constexpr size_t countReader(int, IdentifierT< N >)
	{
		return N;
	}

	//Now this will recursively keep trying smaller numbers until it finds
	//the first adl_lookup that is defined.
	//We are limited in terms of recursion by the number of template recursions
	//allowed by the compiler. This can be improved to do a binary search if we
	//do hit that so we could support 2^n versus just n
	template< size_t N >
	static constexpr size_t countReader(
		float, IdentifierT< N >, size_t R = countReader(0, IdentifierT< N - 1 >()))
	{
		return R;
	}

	//Stop if we get to 0
	static constexpr size_t countReader(float, IdentifierT< 0 >)
	{
		return 0;
	}

	//--------------------------------------------------------------------------
	// Reads the value on the counter
	//--------------------------------------------------------------------------
	template<size_t Max = 256 >
	static constexpr size_t value(
		size_t R = countReader(0, IdentifierT<Max> {}))
	{
		return R;
	}


	//--------------------------------------------------------------------------
	// Returns the current value & increments the counter 
	// Every time we instantiate InstantiateFriendFunc< T >,
	// we force a definition of friendFunc to "mark/increment" the counter.
	//--------------------------------------------------------------------------
	template<size_t Increment = 1, size_t CurrentValue = value() >
	static constexpr size_t next(
		size_t R = InstantiateFriendFunc< IdentifierT< CurrentValue + Increment > >::value)
	{
		return CurrentValue;
	}
}// end namespace ReflectionAutoRegistration

}//end namespace wgt

#endif // COUNTER_HELPER_HPP