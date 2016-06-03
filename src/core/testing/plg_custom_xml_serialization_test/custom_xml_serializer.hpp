#ifndef CUSTOM_XML_SERIALIZER_HPP
#define CUSTOM_XML_SERIALIZER_HPP
#include "core_serialization/serializer/i_serializer.hpp"

namespace wgt
{
class CustomXmlSerializer : public Implements< ISerializer >
{
public:
	using ISerializer::deserialize;

	CustomXmlSerializer( IDataStream& dataStream );

	virtual bool serialize(const Variant& value) override;
	virtual bool deserialize(Variant& value) override;
	bool sync();
private:
	TextStream stream_;
};
} // end namespace wgt
#endif //CUSTOM_XML_SERIALIZER_HPP
