#include "i_serializer.hpp"


namespace wgt
{
ISerializer::~ISerializer()
{
	// nop
}


bool ISerializer::serialize( const Variant& value )
{
	return false;
}


bool ISerializer::deserialize( Variant& value )
{
	return false;
}


bool ISerializer::write( IDataStream * dataStream, const Variant & variant )
{
	return false;
}


bool ISerializer::read( IDataStream * dataStream, Variant & variant )
{
	return false;
}
} // end namespace wgt
