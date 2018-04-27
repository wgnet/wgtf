#pragma once
#ifndef V0_ISERIALIZERNEW_V0_HPP
#define V0_ISERIALIZERNEW_V0_HPP
#include "../../../lib/core_dependency_system/i_interface.hpp"
#include "../../../lib/core_serialization_new/serializationdocument.hpp"
#include "../../../lib/core_serialization_new/serializationnode.hpp"
#include <memory>

namespace wgt
{
class Variant;
class IDataStream;
class SerializationDocument;
enum SerializationFormat : int;

DECLARE_INTERFACE_BEGIN(ISerializerNew, 0, 0)

virtual std::unique_ptr<SerializationDocument> getDocument(SerializationFormat) = 0;

// Variant Serialization
virtual bool serializeToDocument(const Variant& v, SerializationDocument* doc) = 0;
virtual bool serializeToStream(const Variant& v, IDataStream* stream, SerializationFormat format) = 0;
virtual bool deserializeFromDocument(Variant& v, SerializationDocument* doc) = 0;
virtual bool deserializeFromStream(Variant& v, IDataStream* stream, SerializationFormat format) = 0;

DECLARE_INTERFACE_END()

} // end namespace wgt

#endif // V0_ISERIALIZERNEW_V0_HPP