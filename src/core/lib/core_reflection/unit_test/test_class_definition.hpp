#ifndef TEST_CLASS_DEFINITION_HPP
#define TEST_CLASS_DEFINITION_HPP

namespace wgt
{

class TestBaseObject
{
	DECLARE_REFLECTED

public:
	int value_;

	TestBaseObject() : value_(0)
	{
	}
};


class TestDerivedObject : public TestBaseObject
{
	DECLARE_REFLECTED

public:
	float number_;

	TestDerivedObject() : TestBaseObject(), number_(1.0f)
	{
	}
};


class TestDeepObject : public TestDerivedObject
{
	DECLARE_REFLECTED

public:
	bool deep_;

	TestDeepObject() : deep_(false)
	{
	}
};

class TestRandomObject : public TestBaseObject
{
	DECLARE_REFLECTED

public:
	const int random_;

	TestRandomObject() : TestBaseObject(), random_(rand())
	{
	}

	const int& random() const
	{
		return random_;
	}
};

} // end namespace wgt

#endif //TEST_CLASS_DEFINITION_HPP
