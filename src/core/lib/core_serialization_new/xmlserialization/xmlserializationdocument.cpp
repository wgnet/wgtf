#include "xmlserializationdocument.hpp"
#include "xmlserializationnode.hpp"
#include "../serializationnode.hpp"
#include "../../../lib/core_serialization/i_datastream.hpp"

namespace wgt
{
XMLSerializationDocument::XMLSerializationDocument(SerializerNew* serializer)
    : formatVersion_("0.1"), SerializationDocument(SerializationFormat::XML, serializer)
{
	}

    XMLSerializationDocument::~XMLSerializationDocument()
    {
	}

    bool XMLSerializationDocument::readFromStream(IDataStream* stream)
    {
	    xmlDocument_.Clear();

	    std::streamsize sizeBytes = 0;

	    const int bufferSize = 1000;
	    std::string dataBuffer;
	    char writeBuffer[bufferSize];

	    // Do I need to do explicit casts for these?
	    while (std::streamsize readSize = stream->read(writeBuffer, bufferSize))
	    {
		    dataBuffer.insert(dataBuffer.end(), writeBuffer, writeBuffer + readSize);
		    sizeBytes += readSize;
	    }

	    // Read the stream
	    tinyxml2::XMLError error = xmlDocument_.Parse(dataBuffer.c_str(), static_cast<size_t>(sizeBytes));

	    if (error != tinyxml2::XMLError::XML_SUCCESS)
	    {
		    xmlDocument_.Clear();
		    return false;
	    }

	    stream->seek(0, std::ios_base::beg);

	    return true;
    }

    bool XMLSerializationDocument::writeToStream(IDataStream* stream)
    {
	    tinyxml2::XMLPrinter printer;
	    xmlDocument_.Print(&printer);

	    // Includes null terminator
	    std::streamsize documentLength = static_cast<std::streamsize>(printer.CStrSize());

	    std::streamsize amountRead = stream->write(printer.CStr(), documentLength);
	    if (amountRead != documentLength)
		    return false;

	    stream->seek(0, std::ios_base::beg);

	    return true;
    }

    void XMLSerializationDocument::clear()
    {
	    xmlDocument_.Clear();
    }

    std::unique_ptr<SerializationNode> XMLSerializationDocument::findNode(const char* name)
    {
	    // FirstChildElement handles nullptr (and empty char strings), will just fail anyway
	    if (name == nullptr)
		    return nullptr;

	    auto rootElement = xmlDocument_.RootElement();
	    if (rootElement == nullptr)
	    {
		    return nullptr;
	    }

	    tinyxml2::XMLElement* targetNode = rootElement->FirstChildElement(name);
	    if (targetNode == nullptr)
		    return nullptr;

	    XMLSerializationNode* newNode = new XMLSerializationNode(this, targetNode);
	    return std::unique_ptr<SerializationNode>(newNode);
    }

    // Actually returns the root *element* since there's the xml declaration and format version nodes
    std::unique_ptr<SerializationNode> XMLSerializationDocument::getRootNode()
    {
	    auto rootElement = xmlDocument_.RootElement();
	    if (rootElement == nullptr)
	    {
		    this->initDocument();
		    rootElement = xmlDocument_.RootElement();
	    }

	    SerializationNode* newNode = new XMLSerializationNode(this, rootElement);

	    return std::unique_ptr<SerializationNode>(newNode);
    }

    std::string XMLSerializationDocument::getError() const
    {
	    return std::string(xmlDocument_.ErrorName());
    }

    void XMLSerializationDocument::initDocument()
    {
	    // Write xml declaration
	    auto declaration = xmlDocument_.NewDeclaration();
	    xmlDocument_.InsertFirstChild(declaration);

	    // Get the root node
	    auto rootElement = xmlDocument_.RootElement();
	    if (rootElement == nullptr)
	    {
		    rootElement = xmlDocument_.NewElement(formatData_.rootTag);
		    xmlDocument_.InsertEndChild(rootElement);
	    }

	    // Write the format version into the root element
	    rootElement->SetAttribute(formatData_.formatVersionTag, formatVersion_);
    }

    const char* XMLSerializationDocument::getVersion() const
    {
	    auto rootElement = xmlDocument_.RootElement();
	    if (rootElement == nullptr)
		    return formatVersion_;

	    return rootElement->Attribute(formatData_.formatVersionTag);
    }

    const XMLSerializationDocument::FormatData& XMLSerializationDocument::getFormatData()
    {
	    return formatData_;
    }

} // end namespace wgt