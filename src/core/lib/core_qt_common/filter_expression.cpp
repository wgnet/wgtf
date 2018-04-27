#include "filter_expression.hpp"

#include <algorithm>

#include "core_string_utils/string_utils.hpp"

namespace wgt
{
namespace Detail
{

const char * stristr(const char * haystack, size_t haystackLen, const char * needle, size_t needleLen )
{
	const char * pos = haystack;
	auto endSearch = pos + haystackLen;
	while (pos <= endSearch)
	{
		if (_strnicmp(pos, needle, needleLen) == 0)
		{
			return pos;
		}
		pos++;
	}
	return nullptr;
}


StringRef::size_type find(const StringRef & this_, const StringRef & str )
{
	auto findPos = stristr( this_.data(), this_.length(), str.data(), str.length() );
	if (findPos != nullptr)
	{
		return findPos - str.data();
	}
	return (StringRef::size_type)-1;
}

template<typename TAddTokenFunc>
void tokenize(
	const std::string & str, 
	const char* delimiters, 
	const char* excludeDelimiters, 
	TAddTokenFunc func)
{
	size_t pos0 = 0;
	size_t pos1 = str.find_first_of(delimiters, pos0);
	while (pos1 != std::string::npos)
	{
		auto sublen = (pos1 == std::string::npos ? str.length() : pos1) - pos0;
		StringRef tok = StringRef(str.data() + pos0, sublen);
		if (tok.length() != 0)
		{
			func(tok);
		}

		if (pos1 != std::string::npos)
		{
			StringRef delim(str.data() + pos1, 1);
			if (strchr(excludeDelimiters, *delim.data()) == 0)
			{
				func(delim);
			}
		}

		pos0 = pos1+1;
		pos1 = str.find_first_of(delimiters, pos0);
	}

	if (pos0 < str.size())
	{
		func(StringRef(str.data() + pos0, str.size() - pos0));
	}
}

FilterExpression::Token parseToken(StringRef token)
{
	if (token == "AND" || token == "+")
	{
		return {FilterExpression::Token::AND, token};
	}
	else if (token == "OR" || token == "|")
	{
		return {FilterExpression::Token::OR, token};
	}
	else if (token == "XOR" || token == "^")
	{
		return {FilterExpression::Token::XOR, token};
	}
	else if (token == "NOT" || token == "!" || token == "-")
	{
		return {FilterExpression::Token::NOT, token};
	}
	else if (token == "(")
	{
		return {FilterExpression::Token::GROUP_START, token};
	}
	else if (token == ")")
	{
		return {FilterExpression::Token::GROUP_END, token};
	}
	else
	{
		return {FilterExpression::Token::VALUE, token};
	}
}

bool isBinaryOp(FilterExpression::Token::Type t)
{
	return t == FilterExpression::Token::AND || 
		t == FilterExpression::Token::OR || 
		t == FilterExpression::Token::XOR;
}

bool combineValue(bool a, bool b, FilterExpression::Token::Type t)
{
	switch (t)
	{
	case FilterExpression::Token::AND: return a && b;
	case FilterExpression::Token::OR: return a || b;
	case FilterExpression::Token::XOR: return a ^ b;
	default: return a;
	}
}
} // namespace Detail

FilterExpression::Token::Token(Type t) : type(t), value("") 
{
}

FilterExpression::Token::Token(Type t, StringRef value) : type(t), value(value)
{
}

FilterExpression::FilterExpression()
{

}

FilterExpression::FilterExpression(const StringRef& expression)
{
	reset(expression);
}

void FilterExpression::reset(const StringRef& expression)
{
	expression_ = std::string{expression.data(), expression.length()};
	StringUtils::trim_string(expression_);
	tokens_.clear();

	if (expression_.empty())
	{
		return;
	}

	// Split raw tokens
	const char* kDelimiters = " +-^|!()";
	const char* kExcludeDelimiter = " "; // We handle " " as a special case.

	Detail::tokenize(expression_, kDelimiters, kExcludeDelimiter,
		[this](StringRef token)
	{
		Token parsedToken = Detail::parseToken(token);		
		if (!tokens_.empty())
		{
			auto thisType = parsedToken.type;
			auto lastType = tokens_.back().type;

			// If we have two values next to each other with no operator
			// explicitly defined in between, then insert an implicit OR.
			// If we have a logical NOT, then we invert the OR to an AND.
			// TODO: does any of this make sense?
			if ((thisType == Token::VALUE || thisType == Token::GROUP_START || thisType == Token::NOT) &&
				(lastType == Token::VALUE || lastType == Token::GROUP_END))
			{
				if (thisType == Token::NOT)
				{
					tokens_.emplace_back(Token::AND);
				}
				else
				{
					tokens_.emplace_back(Token::OR);
				}
			}
		}
		tokens_.emplace_back(parsedToken);
	});
}

bool FilterExpression::match(const StringRef& testString) const
{
	if (tokens_.empty())
	{
		return true;
	}

	class TokenEvaluator
	{
	public:
		enum Result
		{
			MATCHED,
			NO_MATCH,
			SYNTAX_ERROR
		};

		TokenEvaluator(const FilterExpression& expr, StringRef testString) : 
			expr_(expr), 
			testString_(testString)
		{}

		Token getLookAhead() const 
		{
			return pos_ < (int)expr_.tokens_.size()-1 ? expr_.tokens_[pos_+1] : end_;
		};

		Token consume()
		{
			return pos_ < (int)expr_.tokens_.size() ? expr_.tokens_[++pos_] : end_;
		};

		Result consumeValue()
		{
			Token tok = consume();
			if (tok.type == Token::END)
			{
				return NO_MATCH;
			}

			bool isMatched = false;
			if (tok.type == Token::NOT)
			{
				isMatched = true;
				tok = consume();
			}
		
			if (tok.type == Token::VALUE)
			{
				if (Detail::find(testString_, tok.value) != (StringRef::size_type)-1)
				{
					isMatched = !isMatched;
				}
			}
			else if (tok.type == Token::GROUP_START)
			{
				Result groupResult = consumeGroup();				
				if (groupResult == MATCHED)
				{
					isMatched = !isMatched;
				}
				else if (groupResult == SYNTAX_ERROR)
				{
					return SYNTAX_ERROR;
				}
			}
			else
			{
				// Syntax error: Unexpected token.
				return SYNTAX_ERROR;
			}

			return isMatched ? MATCHED : NO_MATCH;
		}

		Result consumeGroup()
		{
			// Consume value -> operator -> value -> operator -> value -> ....
			// Unary NOT operator is considered part of value.
			// A group is considered a value and is recursively evaluated.
			Result result = consumeValue();
			if (result == SYNTAX_ERROR)
			{
				return SYNTAX_ERROR;
			}

			while (getLookAhead().type != Token::END)
			{
				Token opToken = consume();
				if (opToken.type == Token::GROUP_END)
				{
					break;
				}

				if (!Detail::isBinaryOp(opToken.type))
				{
					// Syntax error: unexpected token
					return SYNTAX_ERROR;
				}

				Result rhs = consumeValue();
				if (rhs == SYNTAX_ERROR)
				{
					return SYNTAX_ERROR;
				}
				
				bool combination = Detail::combineValue(
					result == MATCHED ? true : false,
					rhs == MATCHED ? true : false,
					opToken.type);

				result = combination ? MATCHED : NO_MATCH;
			}

			return result;
		}

	private:
		const FilterExpression& expr_;
		int pos_ = -1;
		StringRef testString_;
		Token end_{Token::END, ""};
	};

	// Not currently signaling syntax error to user. Just never match if was a problem.
	TokenEvaluator evaluator(*this, testString);
	return evaluator.consumeGroup() == TokenEvaluator::MATCHED ? true : false;
}

size_t FilterExpression::getNumTokens() const
{
	return tokens_.size();
}

const FilterExpression::Token& FilterExpression::getToken(const size_t i) const
{
	return tokens_[i];
}

bool FilterExpression::isEmpty() const
{
	return tokens_.empty();
}

const std::string& FilterExpression::getExpression() const
{
	return expression_;
}

} // namespace wgt