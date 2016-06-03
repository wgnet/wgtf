#include "test_polymorphism.hpp"
#include "core_reflection/utilities/reflection_utilities.hpp"
#include "core_reflection/property_accessor.hpp"
#include "core_reflection/base_property.hpp"

namespace wgt
{
TestPolyCheckBox::TestPolyCheckBox()
	: checked_( false )
{

}

TestPolyTextField::TestPolyTextField()
	: text_( "Wargaming" )
	, num_( 55 )
{

}

void TestPolyTextField::getText( std::string * text ) const
{
	*text = text_;
}

void TestPolyTextField::setText( const std::string & text )
{
	text_ = text;
}

void TestPolyTextField::getNumber( int * num ) const
{
	*num = num_;
}

void TestPolyTextField::setNumber( const int & num )
{
	num_ = num;
}

TestPolyComboBox::TestPolyComboBox()
	: curSelected_( 0 )
{

}

void TestPolyComboBox::getSelected( int * select ) const
{
	*select = curSelected_;
}

void TestPolyComboBox::setSelected( const int & select )
{
	curSelected_ = select;
}

TestPolyColor3::TestPolyColor3()
	: seletColor_( 0 )
{

}

void TestPolyColor3::getSelected( int * select ) const
{
	*select = seletColor_;
}

void TestPolyColor3::setSelected( const int & select )
{
	seletColor_ = select;
}
} // end namespace wgt
