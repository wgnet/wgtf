#ifndef STRONG_TYPE_HPP_INCLUDED
#define STRONG_TYPE_HPP_INCLUDED


/**
Value wrapper used to inhibit implicit conversions.

This template utilizes the fact, that C++ allows at most one implicit conversion
when it tries to match types. And this wrapper "eats" this conversion, so no
more implicit conversion are allowed.

One of examples of using this wrapper is to declare function, that accepts some
exact type, but not any other type, even one that could be implicitly converted.
*/
template< typename T >
class StrongType
{
public:
	StrongType( T v ):
		v_( v )
	{
	}

	T value() const
	{
		return v_;
	}

	operator T() const
	{
		return v_;
	}

private:
	T v_;

};


#endif
