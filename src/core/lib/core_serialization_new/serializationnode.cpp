#include "serializationnode.hpp"
#include "serializationdocument.hpp"

namespace wgt
{
SerializationNode::~SerializationNode()
{
}

// Empty child creation

std::unique_ptr<SerializationNode> SerializationNode::createEmptyChild(const char* childName, size_t nameSize)
{
	if (!checkName(childName, nameSize))
	{
		return nullptr;
	}

	return this->createEmptyChildInternal(childName, nameSize);
}

std::unique_ptr<SerializationNode> SerializationNode::createEmptyChild(const SStringRef& childName)
{
	return this->createEmptyChild(childName.data(), childName.size());
}

// Variant child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildVariant(const char* childName, size_t nameSize,
                                                                         const Variant& v, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	if (!childNode->setValueVariant(v, setType))
	{
		this->deleteChild(childNode);
		return nullptr;
	}

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildVariant(const SStringRef& childName, const Variant& v,
                                                                         bool setType)
{
	return this->createChildVariant(childName.data(), childName.size(), v, setType);
}

// char* child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildString(const char* childName, size_t nameSize,
                                                                        const char* data, size_t dataSize, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueString(data, dataSize, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildString(const SStringRef& childName, const char* data,
                                                                        size_t dataSize, bool setType)
{
	return this->createChildString(childName.data(), childName.size(), data, dataSize, setType);
}

std::unique_ptr<SerializationNode> SerializationNode::createChildString(const char* childName, size_t nameSize,
                                                                        const SStringRef& data, bool setType)
{
	return this->createChildString(childName, nameSize, data.data(), data.size(), setType);
}

std::unique_ptr<SerializationNode> SerializationNode::createChildString(const SStringRef& childName,
                                                                        const SStringRef& data, bool setType)
{
	return this->createChildString(childName.data(), childName.size(), data.data(), data.size(), setType);
}

// char* child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildWString(const char* childName, size_t nameSize,
                                                                         const wchar_t* data, size_t dataSize,
                                                                         bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueWString(data, dataSize, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildWString(const SStringRef& childName,
                                                                         const wchar_t* data, size_t dataSize,
                                                                         bool setType)
{
	return this->createChildWString(childName.data(), childName.size(), data, dataSize, setType);
}

std::unique_ptr<SerializationNode> SerializationNode::createChildWString(const char* childName, size_t nameSize,
                                                                         const WSStringRef& data, bool setType)
{
	return this->createChildWString(childName, nameSize, data.data(), data.size(), setType);
}

std::unique_ptr<SerializationNode> SerializationNode::createChildWString(const SStringRef& childName,
                                                                         const WSStringRef& data, bool setType)
{
	return this->createChildWString(childName.data(), childName.size(), data.data(), data.size(), setType);
}

// double child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildDouble(const SStringRef& childName, double value,
                                                                        bool setType)
{
	auto childNode = this->createEmptyChild(childName);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueDouble(value, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildDouble(const char* childName, size_t nameSize,
                                                                        double value, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueDouble(value, setType);

	return childNode;
}

// intmax_t child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildInt(const SStringRef& childName, intmax_t value,
                                                                     bool setType)
{
	auto childNode = this->createEmptyChild(childName);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueInt(value, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildInt(const char* childName, size_t nameSize,
                                                                     intmax_t value, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueInt(value, setType);

	return childNode;
}

// uintmax_t child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildUint(const SStringRef& childName, uintmax_t value,
                                                                      bool setType)
{
	auto childNode = this->createEmptyChild(childName);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueUint(value, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildUint(const char* childName, size_t nameSize,
                                                                      uintmax_t value, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueUint(value, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildChar(const SStringRef& childName, char value,
                                                                      bool setType)
{
	return this->createChildChar(childName.data(), childName.size(), value, setType);
}

std::unique_ptr<SerializationNode> SerializationNode::createChildChar(const char* childName, size_t nameSize,
                                                                      char value, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueChar(value, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildWChar(const SStringRef& childName, wchar_t value,
                                                                       bool setType)
{
	return this->createChildWChar(childName.data(), childName.size(), value, setType);
}

std::unique_ptr<SerializationNode> SerializationNode::createChildWChar(const char* childName, size_t nameSize,
                                                                       wchar_t value, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueWChar(value, setType);

	return childNode;
}

// bool child creation

std::unique_ptr<SerializationNode> SerializationNode::createChildBool(const SStringRef& childName, bool value,
                                                                      bool setType)
{
	auto childNode = this->createEmptyChild(childName);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueBool(value, setType);

	return childNode;
}

std::unique_ptr<SerializationNode> SerializationNode::createChildBool(const char* childName, size_t nameSize,
                                                                      bool value, bool setType)
{
	auto childNode = this->createEmptyChild(childName, nameSize);
	if (childNode == nullptr)
	{
		return nullptr;
	}

	childNode->setValueBool(value, setType);

	return childNode;
}

// Set name

void SerializationNode::setName(const char* name, size_t nameSize)
{
	if (!checkName(name, nameSize))
	{
		return;
	}

	this->setNameInternal(name, nameSize);
}

void SerializationNode::setName(const SStringRef& name)
{
	this->setName(name.data(), name.size());
}

// Set value as variant

bool SerializationNode::setValueVariant(const Variant& v, bool setType)
{
	// Create a temporary unique_ptr to use serializeObject
	auto tempPtr = std::unique_ptr<SerializationNode>(this);
	bool success = document_->getSerializer()->serializeObject(v, tempPtr, setType);
	tempPtr.release();
	return success;
}

bool SerializationNode::getValueVariant(Variant& v, const char* typeName)
{
	auto tempPtr = std::unique_ptr<SerializationNode>(this);
	bool success = document_->getSerializer()->deserializeObject(v, tempPtr, typeName);
	tempPtr.release();
	return success;
}

// Get document

SerializationDocument* SerializationNode::getDocument() const
{
	return document_;
}

// Name validation

bool SerializationNode::checkName(const char* name, size_t nameSize)
{
	// Using narrow chars
	std::string nameString(name, nameSize);

	bool success = true;

	success = (success) && checkNameFirstChar(nameString);

	success = (success) && checkNameXML(nameString);

	success = (success) && checkNameSymbols(nameString);

	return success;
}

bool SerializationNode::checkName(const SStringRef& name)
{
	return checkName(name.data(), name.size());
}

bool SerializationNode::checkNameXML(std::string name)
{
	if (name[0] == 'X' || name[0] == 'x')
	{
		if (name[1] == 'M' || name[1] == 'm')
		{
			if (name[2] == 'L' || name[2] == 'l')
			{
				return false;
			}
		}
	}

	return true;
}

bool SerializationNode::checkNameFirstChar(std::string name)
{
	if (name.empty())
	{
		// XML names are not allowed to be empty!
		return false;
	}

	// The first character of a name must be A-Z, a-z, or _

	char& firstChar = name[0];

	// See if it's alphanumeric
	if (firstChar < 128 && isalpha(firstChar))
	{
		return true;
	}

	// See if it's an underscore
	if (firstChar == 95)
	{
		return true;
	}

	// Invalid
	return false;
}

bool SerializationNode::checkNameSymbols(std::string name)
{
	if (name.empty())
	{
		return false;
	}

	auto it = name.begin();
	++it;

	// Names may only contain letters, digits, hyphens, underscores and periods (cannot be a space) - XML requirements
	for (auto end = name.end(); it != end; ++it)
	{
		char& c = *it;

		// Check that it's alphanumeric, an underscore, a hyphen, or a period.
		if ((c < 128 && isalnum(c)) || c == 95 || c == 45 || c == 46)
		{
			continue;
		}

		// It's not a valid character.
		return false;
	}

	return true;
}

} // end namespace wgt