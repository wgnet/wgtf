#ifndef FILTER_EXPRESSION_HPP
#define FILTER_EXPRESSION_HPP

#include "wg_types/string_ref.hpp"

#include <QString>

#include <vector>

namespace wgt
{
class FilterExpression
{
public:
	FilterExpression();
	FilterExpression(const StringRef& expression);

	/*
		Resets the expression represented by this FilterExpression instance,
		by tokenizing the given string. This will not report any syntactical
		errors, since FilterExpression is an "interpreter" and won't know if
		there's a problem until you call match().
	*/
	void reset(const StringRef& expression);

	/*
		Performs a a test against the given string to see if there
		is a match. Always returns false (i.e. no match) if there is
		a syntax error.
	*/
	bool match(const StringRef& testString) const;

	/*
		Returns if the expression is currently empty (i.e. equivalent to "" expression).
	*/
	bool isEmpty() const;

	/*
		Returns the active expression string as per the last call to reset().
	*/
	const std::string& getExpression() const;

public:
	struct Token
	{
		enum Type
		{
			VALUE,
			GROUP_START,
			GROUP_END,
			AND,
			OR,
			XOR,
			NOT,
			END
		};

		Type type;
		StringRef value;

		////

		Token(Type t);
		Token(Type t, StringRef value);
	};

	size_t getNumTokens() const;
	const Token& getToken(const size_t i) const;

private:
	std::string expression_;
	std::vector<Token> tokens_;
};
} // end namespace wgt

#endif // FILTER_EXPRESSION_HPP