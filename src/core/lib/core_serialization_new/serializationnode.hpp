#pragma once
#include "../../lib/core_variant/variant.hpp"
#include "serializationhandler.hpp"
#include <memory>
#include <vector>

namespace wgt
{
class SerializationDocument;
class Variant;

/**Serialization String Reference. Used for increased safety with char literals.
*/
template <class C>
class BaseStringRef
{
public:

	// Constructor for string literals
	template <size_t N>
	BaseStringRef(C const (&data)[N])
	{
		size_ = N - 1;
		data_ = data;
	}

	// Constructor for strings/vectors
	template <class T, typename = std::is_void<typename T::value_type>>
	BaseStringRef(const T& src)
	{
		size_ = src.size();
		data_ = src.data();
	}

	// Prevent creation of SStringRef from char*
	explicit BaseStringRef(const C*) = delete;
	explicit BaseStringRef(C*) = delete;

	const C* data() const
	{
		return data_;
	}

	size_t size() const
	{
		return size_;
	}

private:
	const C* data_ = nullptr;
	size_t size_ = 0;
};

typedef BaseStringRef<char> SStringRef;
typedef BaseStringRef<wchar_t> WSStringRef;

/**
* SerializationNode is a format-agnostic wrapper for the nodes of serialization DOM implementations.
*/
class SerializationNode
{
	friend class SerializerNew;
	friend class SerializationDocument;

	typedef std::unique_ptr<SerializationNode> NodePtr;

public:
	SerializationNode() = delete;
	virtual ~SerializationNode() = 0;

	/**If a child already exists with the given name, a second child is created with the same name. These children can
	 be accessed through getAllChildren. (Binary/json/etc may not support identical child names, but the implementation
	 must still provide the same functionality.*/
	NodePtr createEmptyChild(const char* childName, size_t nameSize);
	NodePtr createEmptyChild(const SStringRef& childName);

	// Specific -Variant suffix to allow compiler to find the correct function.
	NodePtr createChildVariant(const char* childName, size_t nameSize, const Variant& v, bool setType = true);
	NodePtr createChildVariant(const SStringRef& childName, const Variant& v, bool setType = true);

	// char* data serialization with handling for null terminators.
	// Pointer type, so we cannot automatically handle memory management at this stage (waiting on ObjectHandle update)/
	NodePtr createChildString(const char* childName, size_t nameSize, const char* data, size_t dataSize,
	                          bool setType = true);
	NodePtr createChildString(const SStringRef& childName, const char* data, size_t dataSize, bool setType = true);
	NodePtr createChildString(const char* childName, size_t nameSize, const SStringRef& data, bool setType = true);
	NodePtr createChildString(const SStringRef& childName, const SStringRef& data, bool setType = true);

	// wchar_t* data serialization
	NodePtr createChildWString(const char* childName, size_t nameSize, const wchar_t* data, size_t dataSize,
	                           bool setType = true);
	NodePtr createChildWString(const SStringRef& childName, const wchar_t* data, size_t dataSize, bool setType = true);
	NodePtr createChildWString(const char* childName, size_t nameSize, const WSStringRef& data, bool setType = true);
	NodePtr createChildWString(const SStringRef& childName, const WSStringRef& data, bool setType = true);

	// Other primitive type createChild functions
	NodePtr createChildDouble(const SStringRef& childName, double value, bool setType = true);
	NodePtr createChildDouble(const char* childName, size_t nameSize, double value, bool setType = true);
	NodePtr createChildInt(const SStringRef& childName, intmax_t value, bool setType = true);
	NodePtr createChildInt(const char* childName, size_t nameSize, intmax_t value, bool setType = true);
	NodePtr createChildUint(const SStringRef& childName, uintmax_t value, bool setType = true);
	NodePtr createChildUint(const char* childName, size_t nameSize, uintmax_t value, bool setType = true);
	NodePtr createChildChar(const SStringRef& childName, char value, bool setType = true);
	NodePtr createChildChar(const char* childName, size_t nameSize, char value, bool setType = true);
	NodePtr createChildWChar(const SStringRef& childName, wchar_t value, bool setType = true);
	NodePtr createChildWChar(const char* childName, size_t nameSize, wchar_t value, bool setType = true);
	NodePtr createChildBool(const SStringRef& childName, bool value, bool setType = true);
	NodePtr createChildBool(const char* childName, size_t nameSize, bool value, bool setType = true);

	virtual NodePtr getChildNode(const char* childName, size_t nameSize) = 0;
	virtual NodePtr getChildNode(const SStringRef& childName) = 0;

	virtual std::vector<NodePtr> getAllChildren(const char* childName = "", size_t nameSize = size_t(0)) = 0;
	virtual std::vector<NodePtr> getAllChildren(const SStringRef& childName) = 0;

	virtual bool isNull() const = 0;

	virtual std::string getName() const = 0;
	virtual std::string getType() const = 0;
	virtual std::string getHandlerName() const = 0;

	// Primitive type getValues
	virtual std::string getValueString() const = 0;
	virtual std::wstring getValueWString() const = 0;
	virtual double getValueDouble() const = 0;
	virtual intmax_t getValueInt() const = 0;
	virtual uintmax_t getValueUint() const = 0;
	virtual char getValueChar() const = 0;
	virtual wchar_t getValueWChar() const = 0;
	virtual bool getValueBool() const = 0;

	/**Names can only be in non-extended ASCII; non-english letters are not supported.
	*Char vectors will fail if they are null-terminated.*/
	void setName(const char* name, size_t nameSize);
	void setName(const SStringRef& name);

	virtual void setHandlerName(const char* handlerName, size_t handlerNameSize) = 0;
	virtual void setHandlerName(const SStringRef& value) = 0;

	// Primitive type setValues
	virtual void setValueString(const char* value, size_t valueSize, bool setType = true) = 0;
	virtual void setValueString(const SStringRef& value, bool setType = true) = 0;
	virtual void setValueWString(const wchar_t* value, size_t valueSize, bool setType = true) = 0;
	virtual void setValueWString(const WSStringRef& value, bool setType = true) = 0;
	virtual void setValueDouble(double value, bool setType = true) = 0;
	virtual void setValueInt(intmax_t value, bool setType = true) = 0;
	virtual void setValueUint(uintmax_t value, bool setType = true) = 0;
	virtual void setValueChar(char value, bool setType = true) = 0;
	virtual void setValueWChar(wchar_t value, bool setType = true) = 0;
	virtual void setValueBool(bool value, bool setType = true) = 0;

	// Raw data setValue - requires type name for identification.
	virtual void setValueRawData(const char* value, size_t valueSize, const char* typeName, size_t typeNameSize) = 0;
	virtual void setValueRawData(const char* value, size_t valueSize, const SStringRef& typeName) = 0;

	/**Turns the current node into a serialized variant.*/
	bool setValueVariant(const Variant& v, bool setType = true);
	/**Reads the current node as though it's a serialized variant.*/
	bool getValueVariant(Variant& v, const char* typeName = nullptr);

	virtual void setType(const char* typeName, size_t typeNameSize) = 0;
	virtual void setType(const SStringRef& typeName) = 0;

	virtual void deleteChild(const char* childName, size_t nameSize) = 0;
	virtual void deleteChild(const SStringRef& childName) = 0;
	virtual void deleteChild(NodePtr& childNode) = 0;
	virtual void deleteChildren() = 0;

	SerializationDocument* getDocument() const;

protected:
	SerializationNode(SerializationDocument* doc) : document_(doc)
	{
	}

	virtual NodePtr createEmptyChildInternal(const char* childName, size_t nameSize) = 0;
	virtual NodePtr createEmptyChildInternal(const SStringRef& childName) = 0;

	virtual void setNameInternal(const char* name, size_t nameSize) = 0;
	virtual void setNameInternal(const SStringRef& name) = 0;

private:
	static bool checkName(const char* name, size_t nameSize);
	static bool checkName(const SStringRef& name);

	static bool checkNameXML(const std::string name);
	static bool checkNameFirstChar(const std::string name);
	static bool checkNameSymbols(const std::string name);

	SerializationDocument* document_;
};
}