#include "xmlserializationnode.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include <memory>
#include <sstream>
#include <codecvt>

namespace wgt
{
XMLSerializationNode::~XMLSerializationNode()
{
	}

    std::unique_ptr<SerializationNode> XMLSerializationNode::createEmptyChildInternal(const char* childName,
                                                                                      size_t nameSize)
    {
	    if (childName == nullptr)
	    {
		    return nullptr;
	    }

	    // Use a string to ensure null-terminated char* is passed to tinyxml2
	    std::string name(childName, nameSize);

	    auto doc = xmlNode_->GetDocument();
	    auto childNode = doc->NewElement(name.size() > 0 ? name.c_str() : "");

	    if (childNode == nullptr)
	    {
		    return nullptr;
	    }

	    xmlNode_->InsertEndChild(childNode);

	    return std::move(createNodeInternal(childNode));
    }

    std::unique_ptr<SerializationNode> XMLSerializationNode::createEmptyChildInternal(const SStringRef& childName)
    {
	    return this->createEmptyChildInternal(childName.data(), childName.size());
    }

    std::unique_ptr<SerializationNode> XMLSerializationNode::getChildNode(const char* childName, size_t nameSize)
    {
	    if (childName == nullptr)
	    {
		    return nullptr;
	    }

	    std::string name(childName, nameSize);

	    auto childNode = xmlNode_->FirstChildElement(name.c_str());

	    if (childNode == nullptr)
	    {
		    return nullptr;
	    }

	    return std::move(createNodeInternal(childNode));
    }

    std::unique_ptr<SerializationNode> XMLSerializationNode::getChildNode(const SStringRef& childName)
    {
	    return this->getChildNode(childName.data(), childName.size());
    }

    std::vector<std::unique_ptr<SerializationNode>> XMLSerializationNode::getAllChildren(const char* childName,
                                                                                         size_t nameSize)
    {
	    std::vector<std::unique_ptr<SerializationNode>> childNodes;
	    if (childName == nullptr)
	    {
		    return std::move(childNodes);
	    }

	    std::string nameString(childName, nameSize);
	    const char* childNameSafe = nameString.c_str();

	    // Iterate through children
	    for (tinyxml2::XMLElement* currentChild = xmlNode_->FirstChildElement(); currentChild != nullptr;
	         currentChild = currentChild->NextSiblingElement())
	    {
		    // strcmps all throughout new serialization framework are changed to strncmp in future stashed changes
		    if (strcmp(currentChild->Name(), childNameSafe) == 0 || strcmp(childNameSafe, "") == 0)
		    {
			    childNodes.push_back(this->createNodeInternal(currentChild));
		    }
	    }

	    return std::move(childNodes);
    }

    std::vector<std::unique_ptr<SerializationNode>> XMLSerializationNode::getAllChildren(const SStringRef& childName)
    {
	    return this->getAllChildren(childName.data(), childName.size());
    }

    bool XMLSerializationNode::isNull() const
    {
	    if (xmlNode_ == nullptr)
	    {
		    return true;
	    }

	    return false;
    }

    std::string XMLSerializationNode::getName() const
    {
	    // makeCharSizeString handles nullptrs for us
	    return this->makeCharSizeString(xmlNode_->Name());
    }

    std::string XMLSerializationNode::getValueString() const
    {
	    // makeCharSizeString handles nullptrs for us
	    return this->makeCharSizeString(xmlNode_->GetText());
    }

    std::wstring XMLSerializationNode::getValueWString() const
    {
	    // Get narrow text
	    const char* text = xmlNode_->GetText();
	    if (text == nullptr)
	    {
		    return std::move(std::wstring(L""));
	    }

	    // Convert, handling UTF-8 correctly.
	    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	    std::wstring wText = converter.from_bytes(text);

	    // length() only knows the length in wide chars, as opposed to individual UTF-8 encoded characters.
	    return std::move(wText);
    }

    double XMLSerializationNode::getValueDouble() const
    {
	    std::stringstream s;
	    const char* text = xmlNode_->GetText();
	    // If this was called on the wrong node, then text will be nullptr!
	    if (text == nullptr)
		    return double(0);
	    s << text;
	    double d;
	    s >> d;
	    return d;
    }

    intmax_t XMLSerializationNode::getValueInt() const
    {
	    std::stringstream s;
	    const char* text = xmlNode_->GetText();
	    // If this was called on the wrong node, then text will be nullptr!
	    if (text == nullptr)
		    return intmax_t(0);
	    s << text;
	    intmax_t i;
	    s >> i;
	    return i;
    }

    uintmax_t XMLSerializationNode::getValueUint() const
    {
	    std::stringstream s;
	    const char* text = xmlNode_->GetText();
	    // If this was called on the wrong node, then text will be nullptr!
	    if (text == nullptr)
		    return uintmax_t(0);
	    s << text;
	    uintmax_t u;
	    s >> u;
	    return u;
    }

    char XMLSerializationNode::getValueChar() const
    {
	    std::stringstream s;
	    const char* text = xmlNode_->GetText();
	    // If this was called on the wrong node, then text will be nullptr!
	    if (text == nullptr)
		    return char(0);
	    s << text;
	    char u;
	    s >> u;
	    return u;
    }

    wchar_t XMLSerializationNode::getValueWChar() const
    {
	    // Get narrow text
	    const char* text = xmlNode_->GetText();
	    if (text == nullptr)
	    {
		    return wchar_t(0);
	    }

	    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	    std::wstring wText = converter.from_bytes(text);

	    return wText.front();
    }

    bool XMLSerializationNode::getValueBool() const
    {
	    std::stringstream s;
	    const char* text = xmlNode_->GetText();
	    // If this was called on the wrong node, then text will be nullptr!
	    if (text == nullptr)
		    return false;
	    s << text;
	    bool b;
	    s >> b;
	    return b;
    }

    std::string XMLSerializationNode::getType() const
    {
	    const char* typeTag = xmlNode_->Attribute(this->getDocumentInternal()->getFormatData().typeTag);
	    return this->makeCharSizeString(typeTag);
    }

    std::string XMLSerializationNode::getHandlerName() const
    {
	    const char* handlerNameTag = xmlNode_->Attribute(this->getDocumentInternal()->getFormatData().handlerNameTag);
	    return this->makeCharSizeString(handlerNameTag);
    }

    void XMLSerializationNode::setNameInternal(const char* name, size_t nameSize)
    {
	    if (name == nullptr)
	    {
		    return;
	    }

	    std::string nameString(name, nameSize);

	    xmlNode_->SetName(nameString.c_str() ? nameString.c_str() : "");
    }

    void XMLSerializationNode::setNameInternal(const SStringRef& name)
    {
	    this->setNameInternal(name.data(), name.size());
    }

    void XMLSerializationNode::setValueString(const char* value, size_t valueSize, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().stringName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::string valueString(value, valueSize);
	    xmlNode_->SetText(valueString.c_str());
    }

    void XMLSerializationNode::setValueString(const SStringRef& value, bool setType)
    {
	    this->setValueString(value.data(), value.size());
    }

    void XMLSerializationNode::setValueWString(const wchar_t* value, size_t valueSize, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().wstringName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::wstring valueString(value, valueSize);
	    // Convert possible UTF-8 encoding to narrow char*
	    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	    std::string narrowValue = converter.to_bytes(valueString);

	    xmlNode_->SetText(narrowValue.c_str());
    }

    void XMLSerializationNode::setValueWString(const WSStringRef& value, bool setType)
    {
	    this->setValueWString(value.data(), value.size());
    }

    void XMLSerializationNode::setValueDouble(double value, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().doubleName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::stringstream ss;
	    ss << value;
	    std::string s;
	    ss >> s;
	    xmlNode_->SetText(s.c_str());
    }

    void XMLSerializationNode::setValueInt(intmax_t value, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().intName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::stringstream ss;
	    ss << value;
	    std::string s;
	    ss >> s;
	    xmlNode_->SetText(s.c_str());
    }

    void XMLSerializationNode::setValueUint(uintmax_t value, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().uintName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::stringstream ss;
	    ss << value;
	    std::string s;
	    ss >> s;
	    xmlNode_->SetText(s.c_str());
    }

    void XMLSerializationNode::setValueChar(char value, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().charName;
		    this->setType(typeName, strlen(typeName));
	    }

	    char tmp[2] = {};
	    tmp[0] = value;

	    xmlNode_->SetText(tmp);
    }

    void XMLSerializationNode::setValueWChar(wchar_t value, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().wstringName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	    std::string narrowValue = converter.to_bytes(value);

	    xmlNode_->SetText(narrowValue.c_str());
    }

    void XMLSerializationNode::setValueBool(bool value, bool setType)
    {
	    if (setType)
	    {
		    const char* typeName = this->getDocument()->getPrimitiveNames().boolName;
		    this->setType(typeName, strlen(typeName));
	    }

	    std::stringstream ss;
	    ss << value;
	    std::string s;
	    ss >> s;
	    xmlNode_->SetText(s.c_str());
    }

    void XMLSerializationNode::setValueRawData(const char* value, size_t valueSize, const char* typeName,
                                               size_t typeNameSize)
    {
	    if (typeName != nullptr)
	    {
		    this->setType(typeName, typeNameSize);
	    }

	    std::string nameString(value, valueSize);

	    xmlNode_->SetText(nameString.c_str());
    }

    void XMLSerializationNode::setValueRawData(const char* value, size_t valueSize, const SStringRef& typeName)
    {
	    setValueRawData(value, valueSize, typeName.data(), typeName.size());
    }

    void XMLSerializationNode::setHandlerName(const char* handlerName, size_t handlerNameSize)
    {
	    if (handlerNameSize == size_t(0))
	    {
		    return;
	    }

	    std::string handlerNameString(handlerName, handlerNameSize);

	    xmlNode_->SetAttribute(this->getDocumentInternal()->getFormatData().handlerNameTag, handlerNameString.c_str());
    }

    void XMLSerializationNode::setHandlerName(const SStringRef& value)
    {
	    this->setHandlerName(value.data(), value.size());
    }

    void XMLSerializationNode::deleteChild(const char* childName, size_t nameSize)
    {
	    if (childName == nullptr)
	    {
		    return;
	    }

	    std::string nameString(childName, nameSize);

	    auto targetChild = xmlNode_->FirstChildElement(nameString.c_str());

	    if (targetChild == nullptr)
	    {
		    return;
	    }

	    xmlNode_->DeleteChild(targetChild);
    }

    void XMLSerializationNode::deleteChild(const SStringRef& childName)
    {
	    this->deleteChild(childName.data(), childName.size());
    }

    void XMLSerializationNode::deleteChild(std::unique_ptr<SerializationNode>& childNode)
    {
	    // Ensure that we have the same document (and must therefore be of the same type)
	    if (childNode->getDocument() != this->getDocument())
	    {
		    return;
	    }

	    auto childNodeXML = static_cast<XMLSerializationNode*>(childNode.get());
	    tinyxml2::XMLElement* internalXMLNode = childNodeXML->getInternalNode();

	    // Iterate through child XML nodes
	    bool found = false;
	    auto currentNode = xmlNode_->FirstChildElement();
	    auto lastNode = xmlNode_->LastChildElement();

	    while (currentNode != lastNode)
	    {
		    if (currentNode == internalXMLNode)
		    {
			    found = true;
			    break;
		    }
		    currentNode = currentNode->NextSiblingElement();
	    }

	    // childNode isn't actually a child
	    if (!found)
	    {
		    return;
	    }

	    xmlNode_->DeleteChild(internalXMLNode);
	    childNode.reset();
    }

    void XMLSerializationNode::deleteChildren()
    {
	    xmlNode_->DeleteChildren();
    }

    tinyxml2::XMLElement* XMLSerializationNode::getInternalNode()
    {
	    return xmlNode_;
    }

    void XMLSerializationNode::setType(const char* typeName, size_t nameSize)
    {
	    std::string nameString(typeName, nameSize);
	    xmlNode_->SetAttribute(this->getDocumentInternal()->getFormatData().typeTag,
	                           nameString.c_str() ? nameString.c_str() : "");
    }

    void XMLSerializationNode::setType(const SStringRef& typeName)
    {
	    this->setType(typeName.data(), typeName.size());
    }

    bool operator==(XMLSerializationNode& lhs, XMLSerializationNode& rhs)
    {
	    return lhs.getInternalNode() == rhs.getInternalNode();
    }

    std::unique_ptr<SerializationNode> XMLSerializationNode::createNodeInternal(tinyxml2::XMLElement* node)
    {
	    SerializationNode* newNode = new XMLSerializationNode((XMLSerializationDocument*)this->getDocument(), node);

	    return std::unique_ptr<SerializationNode>(newNode);
    }

    XMLSerializationDocument* XMLSerializationNode::getDocumentInternal() const
    {
	    // XMLNodes are only created by an XMLDocument or other XMLNodes, so this should always be valid.
	    return (XMLSerializationDocument*)this->getDocument();
    }

    std::string XMLSerializationNode::makeCharSizeString(const char* charData) const
    {
	    if (charData == nullptr)
	    {
		    return std::string("");
	    }

	    // tinyxml2 returns null terminated char*
	    size_t count = 0;
	    while (charData[count] != 0)
	    {
		    ++count;
	    }

	    return std::move(std::string(charData, count));
    }

} // end namespace wgt