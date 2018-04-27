#include "pch.hpp"

#include "core_unit_test/unit_test.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_dll.hpp"
#include "core_string_utils/file_path.hpp"
#include "core_string_utils/string_utils.hpp"
#include <array>
#include <fstream>
#include <QDir>
#include <QStringList>

#include "core_qt_common/filter_expression.hpp"

namespace wgt
{
struct TestToken
{
	FilterExpression::Token::Type type = FilterExpression::Token::END;
	const char* value = "";

	TestToken(){}
	TestToken(FilterExpression::Token::Type t) : type(t) {}
	TestToken(FilterExpression::Token::Type t, const char* v) : type(t), value(v) {}
};

bool validateTokens(const FilterExpression& expr, std::vector<TestToken> expected)
{
	if (expr.getNumTokens() != expected.size())
	{
		return false;
	}

	for (size_t i = 0; i < expected.size(); ++i)
	{
		if (expr.getToken(i).type != expected[i].type)
		{
			return false;
		}

		if (expected[i].value[0] != '\0' &&
			expr.getToken(i).value != expected[i].value)
		{
			return false;
		}
	}

	return true;
}

TEST(testFilterExpression)
{
	CHECK(validateTokens(FilterExpression(""), {}));
	CHECK(validateTokens(FilterExpression(" "), {}));
	CHECK(validateTokens(FilterExpression("    "), {}));
	CHECK(validateTokens(FilterExpression("\t\t"), {}));
	CHECK(validateTokens(FilterExpression("\t \t"), {}));

	CHECK(validateTokens(FilterExpression("test"), 
	{
		{FilterExpression::Token::VALUE, "test"}
	}));

	CHECK(validateTokens(FilterExpression(" test"), 
	{
		{FilterExpression::Token::VALUE, "test"}
	}));

	CHECK(validateTokens(FilterExpression("test "), 
	{
		{FilterExpression::Token::VALUE, "test"}
	}));

	CHECK(validateTokens(FilterExpression(" test "), 
	{
		{FilterExpression::Token::VALUE, "test"}
	}));

	CHECK(validateTokens(FilterExpression("   test    "), 
	{
		{FilterExpression::Token::VALUE, "test"}
	}));

	CHECK(validateTokens(FilterExpression("  \ttest\t \t"), 
	{
		{FilterExpression::Token::VALUE, "test"}
	}));
	
	CHECK(validateTokens(FilterExpression("A(B+C-D^E)F"), 
		{
			FilterExpression::Token::VALUE,
			FilterExpression::Token::OR,
			FilterExpression::Token::GROUP_START,
			FilterExpression::Token::VALUE,
			FilterExpression::Token::AND,
			FilterExpression::Token::VALUE,
			FilterExpression::Token::AND,
			FilterExpression::Token::NOT,
			FilterExpression::Token::VALUE,
			FilterExpression::Token::XOR,
			FilterExpression::Token::VALUE,
			FilterExpression::Token::GROUP_END,
			FilterExpression::Token::OR,
			FilterExpression::Token::VALUE
		}));

	CHECK(validateTokens(FilterExpression("A (B +C -D) E"), 
		{
			FilterExpression::Token::VALUE, 
			FilterExpression::Token::OR, 
			FilterExpression::Token::GROUP_START, 
			FilterExpression::Token::VALUE,
			FilterExpression::Token::AND, 
			FilterExpression::Token::VALUE, 
			FilterExpression::Token::AND, 
			FilterExpression::Token::NOT, 
			FilterExpression::Token::VALUE,
			FilterExpression::Token::GROUP_END, 
			FilterExpression::Token::OR, 
			FilterExpression::Token::VALUE
		}));

	{
		auto expr = FilterExpression("");

		// Should match anything
		CHECK(expr.match(""));
		CHECK(expr.match("test"));
		CHECK(expr.match("foo bar"));
	}

	{
		auto expr = FilterExpression("test");
		CHECK(validateTokens(expr, 
			{
				{FilterExpression::Token::VALUE, ""}
			}));

		CHECK(!expr.match(""));
		CHECK(!expr.match("foo"));
		CHECK(expr.match("test"));
		CHECK(expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("NOT test");
		CHECK(validateTokens(expr, {
			FilterExpression::Token::NOT, 
			{FilterExpression::Token::VALUE, "test"}}));

		CHECK(expr.match(""));
		CHECK(expr.match("foo"));
		CHECK(!expr.match("test"));
		CHECK(!expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("foo bar");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::OR, 
			{FilterExpression::Token::VALUE, "bar"}}));

		CHECK(!expr.match(""));
		CHECK(expr.match("foo"));
		CHECK(expr.match("bar"));
		CHECK(expr.match("a foobar string"));
		CHECK(!expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("foo NOT bar");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::AND, 
			FilterExpression::Token::NOT, 
			{FilterExpression::Token::VALUE, "bar"}}));
		
		CHECK(!expr.match(""));
		CHECK(!expr.match("foo bar"));
		CHECK(expr.match("foo"));
		CHECK(!expr.match("bar"));
		CHECK(!expr.match("test"));
		CHECK(!expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("foo OR NOT bar");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::OR, 
			FilterExpression::Token::NOT, 
			{FilterExpression::Token::VALUE, "bar"}}));

		CHECK(expr.match(""));
		CHECK(expr.match("foo"));
		CHECK(expr.match("test"));
		CHECK(!expr.match("bar"));
		CHECK(expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("foo AND bar OR cake");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::AND, 
			{FilterExpression::Token::VALUE, "bar"}, 
			FilterExpression::Token::OR, 
			{FilterExpression::Token::VALUE, "cake"}}));

		CHECK(!expr.match(""));
		CHECK(!expr.match("foo"));
		CHECK(!expr.match("bar"));
		CHECK(expr.match("foo && bar at once!"));
		CHECK(expr.match("or just a cake"));
	}

	{
		auto expr = FilterExpression("foo bar AND cake");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::OR, 
			{FilterExpression::Token::VALUE, "bar"}, 
			FilterExpression::Token::AND, 
			{FilterExpression::Token::VALUE, "cake"}}));

		CHECK(!expr.match(""));
		CHECK(!expr.match("foo bar"));
		CHECK(expr.match("foo cake"));
		CHECK(expr.match("bar cake"));
		CHECK(expr.match("foobar cake"));
		CHECK(!expr.match("cake"));
		CHECK(!expr.match("test"));
		CHECK(!expr.match("a test string"));
	}

	{
		auto expr = FilterExpression(" ( test ) ");
		CHECK(validateTokens(expr, 
			{FilterExpression::Token::GROUP_START,
			{FilterExpression::Token::VALUE, "test"},
			FilterExpression::Token::GROUP_END}));

		CHECK(!expr.match(""));
		CHECK(!expr.match("foo"));
		CHECK(expr.match("test"));
		CHECK(expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("foo OR (bar AND cake)");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::OR,
			FilterExpression::Token::GROUP_START,
			{FilterExpression::Token::VALUE, "bar"}, 
			FilterExpression::Token::AND, 
			{FilterExpression::Token::VALUE, "cake"},
			FilterExpression::Token::GROUP_END}));
		
		CHECK(!expr.match(""));
		CHECK(expr.match("foo bar"));
		CHECK(expr.match("foo bar cake"));
		CHECK(!expr.match("bar pie"));
		CHECK(expr.match("foo bar pie"));
		CHECK(!expr.match("cake"));
		CHECK(!expr.match("test"));
		CHECK(!expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("foo XOR (bar AND cake)");
		CHECK(validateTokens(expr, {
			{FilterExpression::Token::VALUE, "foo"}, 
			FilterExpression::Token::XOR,
			FilterExpression::Token::GROUP_START,
			{FilterExpression::Token::VALUE, "bar"},
			FilterExpression::Token::AND, 
			{FilterExpression::Token::VALUE, "cake"},
			FilterExpression::Token::GROUP_END}));
		

		
		CHECK(!expr.match(""));
		CHECK(expr.match("foo bar"));
		CHECK(!expr.match("foo bar cake"));
		CHECK(!expr.match("bar pie"));
		CHECK(expr.match("foo bar pie"));
		CHECK(expr.match("bar cake"));
		CHECK(!expr.match("cake"));
		CHECK(!expr.match("test"));
		CHECK(!expr.match("a test string"));
	}

	{
		auto expr = FilterExpression("test tank AND (foo ^ (pie OR cake))");
		CHECK(validateTokens(expr, 
			{
				{FilterExpression::Token::VALUE, "test"}, 
				FilterExpression::Token::OR, 
				{FilterExpression::Token::VALUE, "tank"}, 
				FilterExpression::Token::AND,
				FilterExpression::Token::GROUP_START,
				{FilterExpression::Token::VALUE, "foo"}, 
				FilterExpression::Token::XOR,
				FilterExpression::Token::GROUP_START,
				{FilterExpression::Token::VALUE, "pie"}, 
				FilterExpression::Token::OR, 
				{FilterExpression::Token::VALUE, "cake"},
				FilterExpression::Token::GROUP_END,
				FilterExpression::Token::GROUP_END
		}));

		CHECK(!expr.match(""));
		CHECK(!expr.match("something"));
		CHECK(!expr.match("test foo pie"));
		CHECK(!expr.match("test foo cake"));
		CHECK(!expr.match("tank something"));
		CHECK(expr.match("test pie"));
		CHECK(expr.match("test cake"));		
	}
}
} // end namespace wgt
