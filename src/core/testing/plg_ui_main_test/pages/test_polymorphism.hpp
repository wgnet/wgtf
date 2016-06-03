#ifndef TEST_POLYMORPHISM_HPP
#define TEST_POLYMORPHISM_HPP

#include "core_reflection/reflected_object.hpp"
#include "testing/reflection_objects_test/test_polystruct.hpp"

namespace wgt
{
class TestPolyCheckBox
	: public TestPolyStruct
{
	DECLARE_REFLECTED
public:
	TestPolyCheckBox();
private:
	bool checked_;
};

class TestPolyTextField
	: public TestPolyStruct
{
	DECLARE_REFLECTED
public:
	TestPolyTextField();
private:
	void getText( std::string * text ) const;
	void setText( const std::string & text );

	void getNumber( int * num ) const;
	void setNumber( const int & num );

	std::string text_;
	int num_;
};

class TestPolyComboBox
	: public TestPolyStruct
{
	DECLARE_REFLECTED
public:
	TestPolyComboBox();
private:
	virtual void getSelected( int * select ) const;
	virtual void setSelected( const int & select );

	int curSelected_;
};

class TestPolyColor3
	: public TestPolyComboBox
{
	DECLARE_REFLECTED
public:
	TestPolyColor3();
private:
	virtual void getSelected( int * select ) const;
	virtual void setSelected( const int & select );

	int seletColor_;
};
} // end namespace wgt
#endif // TEST_POLYMORPHISM_HPP
