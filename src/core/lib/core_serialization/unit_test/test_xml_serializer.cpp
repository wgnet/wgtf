#include "pch.hpp"

#include "CppUnitLite2/src/CppUnitLite2.h"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "core_serialization/serializer/xml_serializer.hpp"
#include "core_variant/variant.hpp"
#include "core_variant/collection.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/reflected_types.hpp"
#include <string>
#include <vector>
#include <map>
#include <list>
#include <utility>
#include <memory>
#include <cmath>
#include <limits>

namespace wgt
{
	IDefinitionManager& definitionManager();

	template< typename T >
	const IClassDefinition* classDefinition()
	{
		static const IClassDefinition* s_classDefinitionPtr = nullptr;
		if( !s_classDefinitionPtr )
		{
			s_classDefinitionPtr = definitionManager().registerDefinition< TypeClassDefinition< T > >();
		}

		return s_classDefinitionPtr;
	}

	template< typename T >
	ObjectHandleT<T> createObject()
	{
		return ObjectHandleT<T>(
			std::unique_ptr<T>( new T() ),
			classDefinition<T>() );
	}

	template< typename T, typename Arg0 >
	ObjectHandleT<T> createObject( Arg0&& arg0 )
	{
		return ObjectHandleT<T>(
			std::unique_ptr<T>( new T( std::forward<Arg0>( arg0 ) ) ),
			classDefinition<T>() );
	}


	class SimpleTestObject
	{
		DECLARE_REFLECTED
		friend TextStream& operator<<( TextStream& stream, const SimpleTestObject& value );
		friend TextStream& operator>>( TextStream& stream, SimpleTestObject& value );

	public:
		explicit SimpleTestObject( std::string s = "hello", int i = 42, double d = 1.23456789 ):
			s_( std::move( s ) ),
			i_( i ),
			d_( d )
		{
		}

		bool operator<( const SimpleTestObject& that ) const
		{
			if( s_ < that.s_ )
			{
				return true;
			}

			if( i_ < that.i_ )
			{
				return true;
			}

			if( d_ < that.d_ )
			{
				return true;
			}

			return false;
		}

		bool operator==( const SimpleTestObject& that ) const
		{
			return
				s_ == that.s_ &&
				i_ == that.i_ &&
				std::abs( d_ - that.d_) < 0.0001;
		}

		bool operator!=( const SimpleTestObject& that ) const
		{
			return !( *this == that );
		}

	private:
		std::string s_;
		int i_;
		double d_;

	};

	TextStream& operator<<( TextStream& stream, const SimpleTestObject& value )
	{
		stream << quoted( value.s_ ) << " " << value.i_ << " " << value.d_;
		return stream;
	}

	TextStream& operator>>( TextStream& stream, SimpleTestObject& value )
	{
		stream >> quoted( value.s_ ) >> value.i_ >> value.d_;
		return stream;
	}


	class ComplexTestObject
	{
		DECLARE_REFLECTED

	public:
		ComplexTestObject()
		{
			obj_ = createObject<SimpleTestObject>( "default" );
		}

		void init()
		{
			obj_ = createObject<SimpleTestObject>( "obj_" );
			s_ = "ololo";

			xobj_.clear();
			xobj_.push_back( SimpleTestObject( "object 0" ) );
			xobj_.push_back( SimpleTestObject( "object 1" ) );
			xobj_.push_back( SimpleTestObject( "object 2" ) );

			s_i_.clear();
			s_i_[ "0 zero" ] = 0;
			s_i_[ "1 one" ] = 1;
			s_i_[ "2 two" ] = 2;

			xs_obj_.clear();

			std::map< std::string, ObjectHandle > map;
			map[ "obj0" ] = createObject<SimpleTestObject>( "value 0.0" );
			map[ "obj1" ] = createObject<SimpleTestObject>( "value 0.1" );
			map[ "obj2" ] = createObject<SimpleTestObject>( "value 0.2" );
			xs_obj_.push_back( std::move( map ) );

			map[ "obj0" ] = createObject<SimpleTestObject>( "value 1.0" );
			map[ "obj1" ] = createObject<SimpleTestObject>( "value 1.1" );
			map[ "obj2" ] = createObject<SimpleTestObject>( "value 1.2" );
			xs_obj_.push_back( std::move( map ) );
		}

		bool operator==( const ComplexTestObject& that ) const
		{
			if( *obj_ != *that.obj_ ||
				s_ != that.s_ ||
				xobj_ != that.xobj_ ||
				s_i_ != that.s_i_)
			{
				return false;
			}

			if( xs_obj_.size() != that.xs_obj_.size() )
			{
				return false;
			}

			// vi = vector iterator, tvi = that vector iterator
			for( auto vi = xs_obj_.begin(), tvi = that.xs_obj_.begin(); vi != xs_obj_.end(); ++vi, ++tvi )
			{
				if( vi->size() != tvi->size() )
				{
					return false;
				}

				// mi = map iterator, tmi = that map iterator
				for( auto mi = vi->begin(), tmi = tvi->begin(); mi != vi->end(); ++mi, ++tmi )
				{
					if( mi->first != tmi->first )
					{
						return false;
					}

					auto* p = mi->second.getBase<SimpleTestObject>();
					auto* tp = tmi->second.getBase<SimpleTestObject>();

					if( !p || !tp || *p != *tp )
					{
						return false;
					}
				}
			}

			return true;
		}

		bool operator!=( const ComplexTestObject& that ) const
		{
			return !( *this == that );
		}

		ObjectHandleT<SimpleTestObject> obj_;
		std::string s_;
		std::vector< SimpleTestObject > xobj_;
		std::map< std::string, int > s_i_;
		std::vector< std::map< std::string, ObjectHandle > > xs_obj_;

	};

	IDefinitionManager& definitionManager()
	{
		static IDefinitionManager* s_managerPtr = nullptr;
		if( !s_managerPtr )
		{
			static ObjectManager s_objectManager;
			static DefinitionManager s_definitionManager(s_objectManager);

			s_objectManager.init( &s_definitionManager );

			Reflection::initReflectedTypes( s_definitionManager );

			s_managerPtr = &s_definitionManager;
		}
		return *s_managerPtr;
	}

BEGIN_EXPOSE( SimpleTestObject, MetaNone() )
	EXPOSE( "string", s_ )
	EXPOSE( "integer", i_ )
	EXPOSE( "real", d_ )
END_EXPOSE()


BEGIN_EXPOSE( ComplexTestObject, MetaNone() )
	EXPOSE( "object", obj_ )
	EXPOSE( "string", s_ )
	EXPOSE( "objects", xobj_ )
	EXPOSE( "map_string_to_int", s_i_ )
	EXPOSE( "maps_string_to_obj", xs_obj_ )
END_EXPOSE()

TEST( XMLSerializer_simple )
{
	ResizingMemoryStream dataStream;
	XMLSerializer serializer( dataStream, definitionManager() );

	Variant src = 42;
	CHECK( serializer.serialize( src ) );
	CHECK( serializer.serialize( "hello" ) );

	CHECK( serializer.sync() );
	CHECK_EQUAL( 0, dataStream.seek( 0 ) );

	Variant dst;
	CHECK( serializer.deserialize( dst ) );
	CHECK( dst.canCast< int >() );
	CHECK( dst == 42 );

	std::string s;
	CHECK( serializer.deserialize( s ) );
	CHECK( s == "hello" );
}

TEST(XMLSerializer_void)
{
	ResizingMemoryStream dataStream;
	XMLSerializer serializer(dataStream, definitionManager());

	Variant voidSource(MetaType::get<void>());
	CHECK(voidSource.isVoid());
	CHECK(serializer.serialize(voidSource));

	CHECK(serializer.sync());
	CHECK_EQUAL(0, dataStream.seek(0));

	Variant voidDest;
	CHECK(serializer.deserialize(voidDest));

	CHECK(voidDest.isVoid());
	CHECK(voidDest == voidSource);
}

TEST( XMLSerializer_reflected )
{
	ResizingMemoryStream dataStream;
	XMLSerializer serializer( dataStream, definitionManager() );
	//serializer.setFormat( XMLSerializer::Format::Unformatted() );

	auto srcObjectT = createObject<ComplexTestObject>();
	srcObjectT->init();
	CHECK( serializer.serialize( srcObjectT ) );

	CHECK( serializer.sync() );
	CHECK_EQUAL( 0, dataStream.seek( 0 ) );

	Variant dst;
	CHECK( serializer.deserialize( dst ) );
	ObjectHandle dstObj;
	RETURN_ON_FAIL_CHECK( dst.tryCast( dstObj ) );
	ComplexTestObject* dstTestObj = dstObj.getBase< ComplexTestObject >();
	RETURN_ON_FAIL_CHECK( dstTestObj != nullptr );

	CHECK( *dstTestObj == *srcObjectT );

	auto tmpTestObj = createObject<ComplexTestObject>();
	CHECK( *dstTestObj != *tmpTestObj );

	tmpTestObj->init();
	CHECK( *dstTestObj == *tmpTestObj );

	tmpTestObj->xs_obj_[ 1 ][ "obj1" ] = createObject<SimpleTestObject>( "value 1.1 modified" );
	CHECK( *dstTestObj != *tmpTestObj );
}
} // end namespace wgt
META_TYPE(wgt::SimpleTestObject)
