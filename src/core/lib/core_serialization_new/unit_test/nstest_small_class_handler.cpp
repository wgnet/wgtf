#include "pch.hpp"

#include "nstest_small_class_handler.hpp"
#include "nstest_small_class.hpp"

namespace wgt
{
bool NSTestSmallClassHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	// Set the handler name - important!
	node->setHandlerName(handlerName_);
	node->setType(smallClassName_, strlen(smallClassName_));

	// Cast it to what we know the internal type to be.
	auto object = v.value<NSTestSmallClass>();

	// Set the preferences - just strings, nice and easy.
	node->createChildString("FirstPreference", object.getFirstPref());
	node->createChildString("SecondPreference", object.getSecondPref());
	node->createChildString("ThirdPreference", object.getThirdPref());

	return true;
}

bool NSTestSmallClassHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// Create the internal object for our variant.
	NSTestSmallClass object;

	auto firstNode = node->getChildNode("FirstPreference");
	if (firstNode != nullptr)
	{
		std::string charData = firstNode->getValueString();
		object.setFirstPref(charData.c_str());
	}

	auto secondNode = node->getChildNode("SecondPreference");
	if (secondNode != nullptr)
	{
		std::string charData = secondNode->getValueString();
		object.setSecondPref(charData.c_str());
	}

	auto thirdNode = node->getChildNode("ThirdPreference");
	if (thirdNode != nullptr)
	{
		std::string charData = thirdNode->getValueString();
		object.setThirdPref(charData.c_str());
	}

	v = object;

	return true;
}

const char* NSTestSmallClassHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return smallClassName_;
}

bool NSTestSmallClassHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// If the type matches, then we can handle it.
	const MetaType* foundType = v.type();
	const MetaType* matchType = MetaType::get<NSTestSmallClass>();
	if (foundType == matchType)
	{
		return true;
	}

	return false;
}

bool NSTestSmallClassHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string typeData = node->getType();
	if (typeData.size() > 0)
	{
		// If the file-supplied type (that we set in write) matches, then we can handle it.
		// This check can be performed however you would like, here it's just a string literal for convenience
		// since there's only one type and it's not going to change.
		if (strcmp(typeData.c_str(), smallClassName_) == 0)
		{
			return true;
		}
	}

	return false;
}

} // end namespace wgt