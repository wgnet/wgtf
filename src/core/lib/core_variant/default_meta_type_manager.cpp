#include "default_meta_type_manager.hpp"
#include "core_serialization/fixed_memory_stream.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization/text_stream_manip.hpp"
#include "wg_types/binary_block.hpp"
#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "wg_types/base64.hpp"
#include "meta_type.hpp"
#include "variant.hpp"
#include "collection.hpp"

#include <typeinfo>
#include <cstdint>
#include <string>
#include <memory>


namespace wgt
{
namespace
{

	class VoidMetaType
		: public MetaType
	{
		typedef MetaType base;

		static bool convertToVoid( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
		{
			return true;
		}

	public:
		VoidMetaType():
			base(
				"void",
				MetaType::data< void >( DeducibleFromText ) )
		{
			setDefaultConversionFrom( &convertToVoid );
		}

		void init(void* value) const override
		{
			// nop
		}

		void copy(void* dest, const void* src) const override
		{
			// nop
		}

		void move(void* dest, void* src) const override
		{
			// nop
		}

		void destroy(void* value) const override
		{
			// nop
		}

		bool equal(const void* lhs, const void* rhs) const override
		{
			return true;
		}

		void streamOut(TextStream& stream, const void* value) const override
		{
			stream << "void";
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			stream >> match( "void" );
		}

		void streamOut(BinaryStream& stream, const void* value) const override
		{
			// nop
		}

		void streamIn(BinaryStream& stream, void* value) const override
		{
			// nop
		}

	protected:
		VoidMetaType( const char* name, const MetaType::Data& data ):
			base( name, data )
		{
			setDefaultConversionFrom( &convertToVoid );
		}
	};


	class UIntMetaType:
		public MetaTypeImpl< uintmax_t >
	{
		typedef MetaTypeImpl< uintmax_t > base;

	public:
		UIntMetaType():
			base( "uint", DeducibleFromText )
		{
			addStraightConversion< uintmax_t, intmax_t >();
			addStraightConversion< uintmax_t, double >();
		}
	};


	class IntMetaType:
		public MetaTypeImpl< intmax_t >
	{
		typedef MetaTypeImpl< intmax_t > base;

	public:
		IntMetaType():
			base( "int", DeducibleFromText )
		{
			addStraightConversion< intmax_t, uintmax_t >();
			addStraightConversion< intmax_t, double >();
		}
	};


	class RealMetaType:
		public MetaTypeImpl< double >
	{
		typedef MetaTypeImpl< double > base;

	public:
		RealMetaType():
			base( "real", DeducibleFromText )
		{
			addStraightConversion< double, uintmax_t >();
			addStraightConversion< double, intmax_t >();
		}
	};


	class StringMetaType
		: public MetaTypeImplNoStream<std::string>
	{
		typedef MetaTypeImplNoStream<std::string> base;

		static bool convertToString( const MetaType* toType, void* to, const MetaType* fromType, const void* from )
		{
			ResizingMemoryStream dataStream;
			TextStream stream( dataStream );
			fromType->streamOut( stream, from );
			if( stream.fail() )
			{
				return false;
			}

			stream.sync();
			std::string& toStr = *reinterpret_cast<std::string*>( to );
			toStr = dataStream.takeBuffer();
			return true;
		}

	public:
		StringMetaType():
			base( "string", ForceShared | DeducibleFromText )
		{
			setDefaultConversionFrom( &convertToString );
		}

		void streamOut(TextStream& stream, const void* value) const override
		{
			stream << quoted( base::cast(value) );
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			stream >> quoted( base::cast(value) );
		}

		void streamOut(BinaryStream& stream, const void* value) const override
		{
			stream << base::cast(value);
		}

		void streamIn(BinaryStream& stream, void* value) const override
		{
			stream >> base::cast(value);
		}
	};


	class BinaryBlockMetaType
		: public MetaTypeImplNoStream< BinaryBlock >
	{
		typedef MetaTypeImplNoStream< BinaryBlock > base;

	public:
		BinaryBlockMetaType():
			base( "blob", 0 )
		{
		}

		void streamOut(TextStream& stream, const void* value) const override
		{
			const auto& binary = base::cast(value);
			std::string encodeValue = 
				Base64::encode( static_cast<const char*>(binary.data()), 
								binary.length() );
			FixedMemoryStream dataStream( encodeValue.c_str(), encodeValue.length() );
			stream.serializeString( dataStream );
		}

		void streamIn(TextStream& stream, void* value) const override
		{
			ResizingMemoryStream dataStream;
			stream.deserializeString( dataStream );
			if (stream.fail())
			{
				return;
			}
			std::string decodeValue;
			if(!Base64::decode( dataStream.buffer(), decodeValue ))
			{
				stream.setState( std::ios_base::badbit );
				return;
			}
			base::cast(value) = BinaryBlock(
				decodeValue.c_str(),
				decodeValue.length(),
				false );
		}

		void streamOut(BinaryStream& stream, const void* value) const override
		{
			const auto& binary = base::cast(value);
			stream.serializeBuffer( binary.cdata(), binary.length() );
		}

		void streamIn(BinaryStream& stream, void* value) const override
		{
			ResizingMemoryStream dataStream;
			stream.deserializeBuffer( dataStream );
			if (!stream.fail())
			{
				base::cast(value) = BinaryBlock(
					dataStream.buffer().c_str(),
					dataStream.buffer().length(),
					false );
			}
		}
	};


	const char g_separator = ',';

}

// Vector2

TextStream& operator<<( TextStream& stream, const Vector2& v )
{
	stream << v.x << g_separator << v.y;
	return stream;
}

TextStream& operator>>( TextStream& stream, Vector2& v )
{
	stream >> v.x >> match( g_separator ) >> v.y;
	return stream;
}

BinaryStream& operator<<( BinaryStream& stream, const Vector2& v)
{
	stream << v.x << v.y;
	return stream;
}

BinaryStream& operator>>( BinaryStream& stream, Vector2& v)
{
	stream >> v.x >> v.y;
	return stream;
}

// Vector3

TextStream& operator<<( TextStream& stream, const Vector3& v )
{
	stream << v.x << g_separator << v.y << g_separator << v.z;
	return stream;
}

TextStream& operator>>( TextStream& stream, Vector3& v )
{
	stream >> v.x >> match( g_separator ) >> v.y >> match( g_separator ) >> v.z;
	return stream;
}

BinaryStream& operator<<( BinaryStream& stream, const Vector3& v)
{
	stream << v.x << v.y << v.z;
	return stream;
}

BinaryStream& operator>>( BinaryStream& stream, Vector3& v)
{
	stream >> v.x >> v.y >> v.z;
	return stream;
}

// Vector4

TextStream& operator<<( TextStream& stream, const Vector4& v )
{
	stream << v.x << g_separator << v.y << g_separator << v.z << g_separator << v.w;
	return stream;
}

TextStream& operator>>( TextStream& stream, Vector4& v )
{
	stream >> v.x >> match( g_separator ) >> v.y >> match( g_separator ) >> v.z >> match( g_separator ) >> v.w;
	return stream;
}

BinaryStream& operator<<( BinaryStream& stream, const Vector4& v)
{
	stream << v.x << v.y << v.z << v.w;
	return stream;
}

BinaryStream& operator>>( BinaryStream& stream, Vector4& v)
{
	stream >> v.x >> v.y >> v.z >> v.w;
	return stream;
}


//==============================================================================
DefaultMetaTypeManager::DefaultMetaTypeManager()
	: typeNameToMetaType_()
	, typeInfoToMetaType_()
{
	defaultMetaTypes_.emplace_back( new VoidMetaType() );
	defaultMetaTypes_.emplace_back( new UIntMetaType() );
	defaultMetaTypes_.emplace_back( new IntMetaType() );
	defaultMetaTypes_.emplace_back( new RealMetaType() );
	defaultMetaTypes_.emplace_back( new StringMetaType );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Collection >( "collection" ) );
	defaultMetaTypes_.emplace_back( new BinaryBlockMetaType() );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Vector2 >( "vector2" ) );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Vector3 >( "vector3" ) );
	defaultMetaTypes_.emplace_back( new MetaTypeImpl< Vector4 >( "vector4" ) );

	for( auto it = defaultMetaTypes_.begin(); it != defaultMetaTypes_.end(); ++it )
	{
		registerType( it->get() );
	}
}

//==============================================================================
bool DefaultMetaTypeManager::registerType(const MetaType* type)
{
	bool nameOk = typeNameToMetaType_.emplace(type->name(), type).second;
	bool typeInfoOk = typeInfoToMetaType_.emplace(type->typeId(), type).second;
	return nameOk && typeInfoOk;
}


//==============================================================================
bool DefaultMetaTypeManager::deregisterType(const MetaType* type)
{
	assert( type != nullptr );
	const auto namesErased = typeNameToMetaType_.erase( type->name() );
	const auto typeInfosErased = typeInfoToMetaType_.erase( type->typeId() );
	return (namesErased > 0) && (typeInfosErased > 0);
}


//==============================================================================
const MetaType* DefaultMetaTypeManager::findType(const char* name ) const
{
	auto it = typeNameToMetaType_.find(name);
	if(it != typeNameToMetaType_.end())
	{
		return it->second;
	}

	return nullptr;
}


//==============================================================================
const MetaType* DefaultMetaTypeManager::findType(const TypeId& typeId)  const
{
	auto it = typeInfoToMetaType_.find(typeId);
	if(it != typeInfoToMetaType_.end())
	{
		return it->second;
	}

	return nullptr;
}
} // end namespace wgt
