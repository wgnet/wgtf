#include "pch.hpp"

#include "nstest_big_class_handler.hpp"
#include "nstest_big_class.hpp"

namespace wgt
{
bool NSTestBigClassHandler::write(const Variant& v, const NodePtr& node, bool writeType)
{
	// Set the handlerName so that it can find the right handler when we deserialize it.
	// If we don't do this, then deserialization will fail as this type isn't reflected and won't have a TypeId.
	node->setHandlerName(handlerName_);

	// We know that V is an NSTestBigClass (we checked for it in canHandleInternal).
	// If this type was reflected, we could just use the reflected type handler instead of writing our own.
	// (Unless some of our class's data wasn't reflected and we had to do that ourselves).
	// If our handler worked with multiple classes, we'd check which specific class we were processing here
	// (however we may determine that), and follow a different process for each.
	node->setType(bigClassName_, strlen(bigClassName_));
	auto object = v.value<NSTestBigClass>();

	// We can create a child node for the "name" value and then set the data manually.
	auto nameNode = node->createEmptyChild("name");
	nameNode->setValueString(object.getName());

	// However, it's a lot easier to just use createChild to complete it all in one go
	// This is the approach that you'll generally want to use.
	node->createChildString("string", object.getString());

	// Bear in mind that this only works for primitive types and std::strings
	node->createChildInt("count", object.getCount());

	node->createChildBool("condition", object.getCondition());

	// These functions return the node they create, in case you wanted to use it for something.
	node->createChildDouble("point", object.getPoint());

	// We can pass in any of our custom classes (if they aren't reflected, then they must have a handler!).
	// This line will find our NSTestSmallClassHandler and call write() on that.
	// Non-primitive types can only be serialized through the createChildVariant and setChildVariant functions.
	const Variant child(object.getChild());
	auto childNode = node->createChildVariant("child", child);

	return true;
}

bool NSTestBigClassHandler::read(Variant& v, const NodePtr& node, const char* typeName)
{
	// We know that the node contains an NSTestBigClass, since this wouldn't be called unless canHandle was true
	// If your handler can handle multiple classes, then you'll need to have stored information about which
	// one this node is containing somewhere in the write(..) function, which you can then check here.

	// Create the internal object for our variant.
	NSTestBigClass object;

	// Now we get each property that we set.
	// Currently type versioning isn't implemented, so be careful about changing type defitions, since handlers
	// won't be able to find the old/new data.
	// As a temporary workaround, one could test for both old and new settings, and use whichever ones exist.

	// We get the name_ value.
	auto nameNode = node->getChildNode("name");
	if (nameNode != nullptr)
	{
		std::string name = nameNode->getValueString();
		object.setName(name);
	}

	// Get the string_ value
	auto stringNode = node->getChildNode("string");
	if (stringNode != nullptr)
	{
		std::string value = stringNode->getValueString();
		object.setString(value);
	}

	// Non-char primitive types
	auto countNode = node->getChildNode("count");
	if (countNode != nullptr)
	{
		object.setCount(countNode->getValueInt());
	}

	auto conditionNode = node->getChildNode("condition");
	if (conditionNode != nullptr)
	{
		object.setCondition(conditionNode->getValueBool());
	}

	auto pointNode = node->getChildNode("point");
	if (pointNode != nullptr)
	{
		object.setPoint(pointNode->getValueDouble());
	}

	// And finally, our custom class child node
	auto childNode = node->getChildNode("child");
	if (childNode != nullptr)
	{
		Variant childVariant;
		// If it succeeds, we had a successful read.
		bool success = childNode->getValueVariant(childVariant);
		object.setChild(childVariant.cast<NSTestSmallClass>());
	}

	v = object;

	// Beyond just returning true, you may wish to track failures or the validity of data, or completely stop
	// reading and fail should any data be missing or incorrect.
	// We'll just return true here since the unit test will fail should anything not match up.
	return true;
}

const char* NSTestBigClassHandler::getInternalTypeOf(const Variant& v, const NodePtr& node) const
{
	return bigClassName_;
}

bool NSTestBigClassHandler::canHandleWriteInternal(const Variant& v, bool writeType)
{
	// If the variant-supplied type matches, then we can handle it.
	const MetaType* foundType = v.type();
	const MetaType* matchType = MetaType::get<NSTestBigClass>();
	if (foundType == matchType)
	{
		return true;
	}

	return false;
}

bool NSTestBigClassHandler::canHandleReadInternal(const NodePtr& node, const char* typeName)
{
	std::string typeData = node->getType();
	if (typeData.size() > 0)
	{
		// If the file-supplied type (that we set in write) matches, then we can handle it.
		// This check can be performed however you would like, here it's just a string literal for convenience
		// since there's only one type and it's not going to change.
		if (strcmp(typeData.c_str(), bigClassName_) == 0)
		{
			return true;
		}
	}

	return false;
}

} // end namespace wgt