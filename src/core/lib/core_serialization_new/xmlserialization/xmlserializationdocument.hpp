#pragma once

#include "../serializationdocument.hpp"
#include <tinyxml2/tinyxml2.hpp>

namespace wgt
{
class XMLSerializationDocument : public SerializationDocument
{
public:
	// Defines what the various tags will be called
	struct FormatData
	{
		const char* rootTag = "Root";
		const char* formatVersionTag = "FormatVersion";
		const char* typeTag = "Type";
		const char* handlerNameTag = "HandlerName";
	};

public:
	XMLSerializationDocument(SerializerNew* serializer);
	~XMLSerializationDocument();

	bool readFromStream(IDataStream* stream) override;
	bool writeToStream(IDataStream* stream) override;
	void clear() override;

	NodePtr findNode(const char* name) override;
	NodePtr getRootNode() override;

	std::string getError() const override;

	const char* getVersion() const override;

	const FormatData& getFormatData();

protected:
	void initDocument() override;

private:
	tinyxml2::XMLDocument xmlDocument_;
	const char* formatVersion_;
	FormatData formatData_;
};

} // end namespace wgt