#pragma once
#include "../serializationnode.hpp"
#include "xmlserializationdocument.hpp"
#include <tinyxml2/tinyxml2.hpp>

namespace wgt
{

class XMLSerializationNode : public SerializationNode
{
	typedef std::unique_ptr<SerializationNode> NodePtr;

	friend NodePtr XMLSerializationDocument::findNode(const char*);
	friend NodePtr XMLSerializationDocument::getRootNode();
	friend bool operator==(XMLSerializationNode& lhs, XMLSerializationNode& rhs);

public:
	XMLSerializationNode() = delete;
	~XMLSerializationNode() override;

	NodePtr getChildNode(const char* childName, size_t nameSize) override;
	NodePtr getChildNode(const SStringRef& childName) override;

	std::vector<NodePtr> getAllChildren(const char* childName = "", size_t nameSize = size_t(0)) override;
	std::vector<NodePtr> getAllChildren(const SStringRef& childName) override;

	bool isNull() const override;

	std::string getName() const override;
	std::string getType() const override;
	std::string getHandlerName() const override;

	// Primitive type getValues
	std::string getValueString() const override;
	std::wstring getValueWString() const override;
	double getValueDouble() const override;
	intmax_t getValueInt() const override;
	uintmax_t getValueUint() const override;
	char getValueChar() const override;
	wchar_t getValueWChar() const override;
	bool getValueBool() const override;

	// Primitive type setValues
	void setValueString(const char* value, size_t valueSize, bool setType = true) override;
	void setValueString(const SStringRef& value, bool setType = true) override;
	void setValueWString(const wchar_t* value, size_t valueSize, bool setType = true) override;
	void setValueWString(const WSStringRef& value, bool setType = true) override;
	void setValueDouble(double value, bool setType = true) override;
	void setValueInt(intmax_t value, bool setType = true) override;
	void setValueUint(uintmax_t value, bool setType = true) override;
	void setValueChar(char value, bool setType = true) override;
	void setValueWChar(wchar_t value, bool setType = true) override;
	void setValueBool(bool value, bool setType = true) override;
	void setValueRawData(const char* value, size_t valueSize, const char* typeName, size_t typeNameSize) override;
	void setValueRawData(const char* value, size_t valueSize, const SStringRef& typeName) override;

	void setHandlerName(const char* handlerName, size_t handlerNameSize) override;
	void setHandlerName(const SStringRef& value) override;

	void setType(const char* typeName, size_t typeNameSize) override;
	void setType(const SStringRef& typeName) override;

	void deleteChild(const char* childName, size_t nameSize) override;
	void deleteChild(const SStringRef& childName) override;
	void deleteChild(NodePtr& childNode) override;
	void deleteChildren() override;

protected:
	XMLSerializationNode(XMLSerializationDocument* document, tinyxml2::XMLElement* node)
	    : SerializationNode(document), xmlNode_(node)
	{
	}

	NodePtr createEmptyChildInternal(const char* childName, size_t nameSize) override;
	NodePtr createEmptyChildInternal(const SStringRef& childName) override;

	void setNameInternal(const char* name, size_t nameSize) override;
	void setNameInternal(const SStringRef& name) override;

	tinyxml2::XMLElement* getInternalNode();

private:
	NodePtr createNodeInternal(tinyxml2::XMLElement* node);
	XMLSerializationDocument* getDocumentInternal() const;
	std::string makeCharSizeString(const char* charData) const;

	/**TinyXML2 only accepts null-terminated char* (can't specify a size). Currently worked around by creating an
	std::string, but can be optimized if performance is an issue.*/
	tinyxml2::XMLElement* xmlNode_;
};
}