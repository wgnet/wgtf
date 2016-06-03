#include "test_page.hpp"
#include "interfaces/i_datasource.hpp"
#include "pages/test_polymorphism.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_common/platform_path.hpp"
#include "core_common/platform_dll.hpp"
#include <locale>
#include <codecvt>


namespace wgt
{
TestPage::TestPage()
	: bChecked_( true )
	, boolTest_( false)
	, text_( L"Hello Test" )
	, curSlideData_( 0 )
	, curNum_( 100 )
	, curSelected_( 0 )
	, enumValue_( 0 )
	, vec3_(0.0f, 0.0f, 0.0f)
	, vec4_(0.0f, 0.0f, 0.0f, 0.0f)
	, color3_(192.0f, 192.0f, 192.0f)
	, color4_(127.0f, 127.0f, 127.0f, 127.0f)
	, polyStruct_( nullptr )
	, genericObj_( nullptr )
{
	for( auto i = 0; i < 10; ++i)
	{
		intVector_.push_back( rand() );
	}

	for( auto i = 0; i < 10; ++i)
	{
		std::vector< float > bla;
		for( auto i = 0; i < 10; ++i)
		{
			bla.push_back( (float ) rand()  / RAND_MAX );
		}
		testVector_.push_back( bla );
	}

	for( auto i = 0; i < 10; ++i)
	{
		floatVector_.push_back( ( float ) rand() / RAND_MAX );
	}
	for( auto i = 0; i < 10; ++i)
	{
		std::vector< int > bla;
		for( auto j = 0; j < 10; ++j)
		{
			bla.push_back( rand() );
		}
		vectorVector_.push_back( bla );
	}

	std::string testString;
	for( auto i = 0; i < 10; ++i)
	{
		char testBuffer[ 1024 ];
		char * pos = &testBuffer[ 0 ];
		for( auto j = 0; j < 10; ++j)
		{
			pos += sprintf( pos, "%d%c", rand(), ( char ) ( ( float ) rand() / RAND_MAX * 26 + 'a' ) );
		}
		testString = testBuffer;
		testMap_.insert ( std::make_pair( rand(), testString ) );
	}

	for( auto i = 0; i < 10; ++i)
	{
		std::vector< std::string > stringVector;
		for( auto j = 0; j < 10; ++j)
		{
			char testBuffer[ 1024 ];
			char * pos = &testBuffer[ 0 ];
			for( auto k = 0; k < 10; ++k)
			{
				pos += sprintf( pos, "%d%c", rand(), ( char ) ( ( float ) rand() / RAND_MAX * 26 + 'a' ) );
			}
			stringVector.push_back( testBuffer );
		}
		testVectorMap_.insert ( std::make_pair( rand(), stringVector ) );
	}

	testStringMap_[ "key a" ] = "value a";
	testStringMap_[ "key b" ] = "value b";
	testStringMap_[ "key c" ] = "value c";

	wchar_t path[MAX_PATH];
	::GetModuleFileNameW( NULL, path, MAX_PATH );
	::PathRemoveFileSpecW( path );
	::PathAppendW( path, L"plugins\\" );

	// std::wstring_convert is reported as memory leak - it does custom dtor call and free for codecvt object
	std::unique_ptr<char[]> str( new char[2 * MAX_PATH] );
	wcstombs( str.get(), path, 2 * MAX_PATH );	
	fileUrl_ = str.get();

	fileUrl_ += "plguins_ui.txt";
	assetUrl_ = "file:///sample.png";
}

TestPage::~TestPage()
{
	polyStruct_ = nullptr;
}

void TestPage::init( IDefinitionManager & defManager )
{
	if (polyStruct_ == nullptr)
	{
		polyStruct_ = 
			defManager.create<TestPolyStruct>( false ); 
		polyStruct_->init( defManager );
	}
	if (genericObj_ == nullptr)
	{
		genericObj_ = GenericObject::create( defManager );
		genericObj_->set( "String", std::string( "Wargaming" ) );
		genericObj_->set( "Integer", 100 );
	}
}

void TestPage::setCheckBoxState( const bool & bChecked )
{
	bChecked_ = bChecked;
}
void TestPage::getCheckBoxState( bool * bChecked ) const
{
	*bChecked = bChecked_;
}

void TestPage::setTextField( const std::wstring & text )
{
	text_ = text;
}
void TestPage::getTextField( std::wstring * text ) const
{
	*text = text_;
}

void TestPage::setSlideData( const double & length )
{
	if ((length < this->getSlideMinData()) || (length > this->getSlideMaxData()))
	{
		return;
	}
	curSlideData_ = length;
}
void TestPage::getSlideData(double * length) const
{
	*length = curSlideData_;
}

int TestPage::getSlideMaxData()
{
	return 100;
}
int TestPage::getSlideMinData()
{
	return -100;
}

void TestPage::setNumber( const int & num )
{
	curNum_ = num;
}
void TestPage::getNumber( int * num ) const
{
	*num = curNum_;
}

void TestPage::setSelected( const int & select )
{
	curSelected_ = select;
}
void TestPage::getSelected( int * select ) const
{
	*select = curSelected_;
}

void TestPage::setVector3(const Vector3 & vec3)
{
	vec3_.x = vec3.x;
	vec3_.y = vec3.y;
	vec3_.z = vec3.z;
}
void TestPage::getVector3(Vector3 * vec3) const
{
	vec3->x = vec3_.x;
	vec3->y = vec3_.y;
	vec3->z = vec3_.z;
}

void TestPage::setVector4(const Vector4 & vec4)
{
	vec4_.x = vec4.x;
	vec4_.y = vec4.y;
	vec4_.z = vec4.z;
	vec4_.w = vec4.w;
}
void TestPage::getVector4(Vector4 * vec4) const
{
	vec4->x = vec4_.x;
	vec4->y = vec4_.y;
	vec4->z = vec4_.z;
	vec4->w = vec4_.w;
}

void TestPage::setColor3(const Vector3 & color)
{
	color3_.x = color.x;
	color3_.y = color.y;
	color3_.z = color.z;
}
void TestPage::getColor3(Vector3 * color) const
{
	color->x = color3_.x;
	color->y = color3_.y;
	color->z = color3_.z;
}

void TestPage::setColor4(const Vector4 & color)
{
	color4_.x = color.x;
	color4_.y = color.y;
	color4_.z = color.z;
	color4_.w = color.w;
}
void TestPage::getColor4(Vector4 * color) const
{
	color->x = color4_.x;
	color->y = color4_.y;
	color->z = color4_.z;
	color->w = color4_.w;
}

void TestPage::getThumbnail( std::shared_ptr< BinaryBlock > * thumbnail ) const
{
	auto dataSrcMngr = Context::queryInterface< IDataSourceManager >();
	assert( dataSrcMngr );
	*thumbnail = dataSrcMngr->getThumbnailImage();
}

const GenericObjectPtr & TestPage::getGenericObject() const
{
	return genericObj_;
}

void TestPage::setGenericObject( const GenericObjectPtr & genericObj )
{
	genericObj_ = genericObj;
}

void TestPage::setTestPolyStruct( const TestPolyStructPtr & testPolyStruct )
{
	polyStruct_ = testPolyStruct;
}
const TestPolyStructPtr & TestPage::getTestPolyStruct() const
{
	return polyStruct_;
}

void TestPage::generateEnumFunc(
	std::map< int, std::wstring > * o_enumMap ) const
{
    // change the case just for the purpose of demonstrating dynamically generating dropdown list 
    // when users click on the dropdownbox
    static int i = 0;
    if(i == 0)
    {
        o_enumMap->clear();
        o_enumMap->insert( std::make_pair( 0, L"First Value" ) );
        o_enumMap->insert( std::make_pair( 1, L"Second Value" ) );
        o_enumMap->insert( std::make_pair( 2, L"third Value" ) );
        o_enumMap->insert( std::make_pair( 3, L"Forth Value" ) );
        i = 1;
        return;
    }
    o_enumMap->clear();
    o_enumMap->insert( std::make_pair( 0, L"1st Value" ) );
    o_enumMap->insert( std::make_pair( 1, L"2nd Value" ) );
    o_enumMap->insert( std::make_pair( 2, L"3rd Value" ) );
    o_enumMap->insert( std::make_pair( 3, L"4th Value" ) );
    i = 0;

}

const std::string & TestPage::getFileUrl() const
{
	return fileUrl_;
}

void TestPage::setFileUrl( const std::string & url )
{
	fileUrl_ = url;
}

const std::string & TestPage::getAssetUrl() const
{
	return assetUrl_;
}

void TestPage::setAssetUrl( const std::string & url )
{
	assetUrl_ = url;
}

TestPage2::TestPage2()
	: testPage_( nullptr )
{

}

TestPage2::~TestPage2()
{
	testPage_ = nullptr;
}

void TestPage2::init( IDefinitionManager & defManager )
{
	assert( testPage_ == nullptr );
	testPage_ = defManager.create<TestPage>( false );
	testPage_->init( defManager );
}

const ObjectHandleT<TestPage> & TestPage2::getTestPage() const
{
	return testPage_;
}

void TestPage2::setTestPage( const ObjectHandleT<TestPage> & objHandle )
{
	testPage_ = objHandle;
}
} // end namespace wgt
