#pragma once
#include "serializer_new.hpp"
#include "serializationnode.hpp"

namespace wgt
{
enum SerializationFormat : int;
class SerializerNew;

/**
* ISerializationDocument is a format-agnostic wrapper for serialization DOM implementations.
*/
class SerializationDocument
{
public:
	typedef std::unique_ptr<SerializationNode> NodePtr;

public:
	struct SerializationPrimitiveNames
	{
		const char* boolName = "TFSbool";
		const char* intName = "TFSint";
		const char* uintName = "TFSuint";
		const char* doubleName = "TFSdouble";
		const char* stringName = "TFSstring"; // string and wstring are technically stored as const char* internally,
		const char* wstringName = "TFSwstring"; // but when accessed return a w/string - the difference doesn't matter.
		const char* charName = "TFSchar"; // Single chars
		const char* wcharName = "TFSwchar"; // Single wchars
	};

public:
	SerializationDocument() = delete;
	SerializationDocument(SerializationFormat format, SerializerNew* serializer);
	virtual ~SerializationDocument() = 0;

	virtual bool readFromStream(IDataStream* stream) = 0;
	virtual bool writeToStream(IDataStream* stream) = 0;
	virtual void clear() = 0;

	virtual NodePtr findNode(const char* name) = 0;
	virtual NodePtr getRootNode() = 0;

	virtual const char* getVersion() const = 0;

	virtual std::string getError() const = 0;

	SerializationFormat getFormat() const;
	SerializerNew* const getSerializer() const;

	const SerializationPrimitiveNames& getPrimitiveNames() const;

protected:
	virtual void initDocument() = 0;

private:
	SerializationFormat format_;
	SerializerNew* serializer_;
	SerializationPrimitiveNames primitiveNames_;
};
}