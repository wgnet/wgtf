#include "pch.hpp"

#include "CppUnitLite2/src/CppUnitLite2.h"
#include "core_variant/variant.hpp"
#include "core_serialization/text_stream.hpp"
#include "core_serialization_new/serializer_new.hpp"
#include "core_serialization_new/serializationhandlermanager.hpp"
#include "core_serialization/resizing_memory_stream.hpp"
#include "core_serialization_new/serializationnode.hpp"
#include "core_serialization_new/serializationdocument.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_unit_test/test_object_manager.hpp"

#include "nstest_big_class.hpp"
#include "nstest_small_class.hpp"
#include "nstest_big_class_handler.hpp"
#include "nstest_small_class_handler.hpp"
#include "nstest_reflected_classes.hpp"

#include <memory>
#include <codecvt>

namespace wgt
{
IDefinitionManager& definitionManager()
{
	static IDefinitionManager* s_managerPtr = nullptr;
	if (!s_managerPtr)
	{
		static TestObjectManager s_objectManager;

		static DefinitionManager s_definitionManager(s_objectManager);

		s_objectManager.init(&s_definitionManager);
		registerReflectedTypes(s_definitionManager);

		s_managerPtr = &s_definitionManager;
	}
	return *s_managerPtr;
}

template <typename T>
const IClassDefinition* classDefinition()
{
	static const IClassDefinition* s_classDefinitionPtr = definitionManager().getDefinition<T>();
	if (!s_classDefinitionPtr)
	{
		s_classDefinitionPtr = definitionManager().registerDefinition<TypeClassDefinition<T>>();
	}

	return s_classDefinitionPtr;
}

template <typename T>
ObjectHandleT<T> createObject()
{
	return ObjectHandleT<T>(std::unique_ptr<T>(new T()), classDefinition<T>());
}

template <typename T, typename Arg0>
ObjectHandleT<T> createObject(Arg0&& arg0)
{
	return ObjectHandleT<T>(std::unique_ptr<T>(new T(std::forward<Arg0>(arg0))), classDefinition<T>());
}

TEST(Serializer_New_Primitives_XML)
{
	SerializationHandlerManager handlerManager(definitionManager());
	SerializerNew serializer(&handlerManager);
	auto document = serializer.getDocument(SerializationFormat::XML);
	auto outDocument = serializer.getDocument(SerializationFormat::XML);
	auto testNode = document->getRootNode();
	ResizingMemoryStream rs;

	// Check int
	intmax_t intA = -370;
	intmax_t intB = 0;
	testNode->setValueInt(intA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	intB = outDocument->getRootNode()->getValueInt();
	CHECK(intA == intB);
	intB = 0;
	// Serialize as Variant
	Variant vintA = intA;
	Variant vintB = intB;
	testNode->setValueVariant(vintA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vintB));
	CHECK(vintA == vintB);

	// Check uint
	uintmax_t uintA = 183765;
	uintmax_t uintB = 0;
	testNode->setValueUint(uintA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	uintB = outDocument->getRootNode()->getValueInt();
	CHECK(uintA == uintB);
	uintB = 0;
	// Serialize as Variant
	Variant vuintA = uintA;
	Variant vuintB = uintB;
	testNode->setValueVariant(vuintA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vuintB));
	CHECK(vuintA == vuintB);

	// Check char
	char charA = 117;
	char charB = 0;
	testNode->setValueChar(charA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	charB = outDocument->getRootNode()->getValueChar();
	CHECK(charA == charB);
	charB = 0;
	// Serialize as Variant
	Variant vcharA = charA;
	Variant vcharB = charB;
	testNode->setValueVariant(vcharA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vcharB));
	CHECK(vcharA == vcharB);

	// Check wchar
	wchar_t wcharA = 2856;
	wchar_t wcharB = 0;
	testNode->setValueWChar(wcharA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	wcharB = outDocument->getRootNode()->getValueWChar();
	CHECK(wcharA == wcharB);
	wcharB = 0;
	// Serialize as Variant
	Variant vwcharA = wcharA;
	Variant vwcharB = wcharB;
	testNode->setValueVariant(vwcharA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vwcharB));
	CHECK(vwcharA == vwcharB);

	// Check double
	double doubleA = -42.4242;
	double doubleB = 0;
	testNode->setValueDouble(doubleA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	doubleB = outDocument->getRootNode()->getValueDouble();
	CHECK(doubleA == doubleB);
	doubleB = 0;
	// Serialize as Variant
	Variant vdoubleA = doubleA;
	Variant vdoubleB = doubleB;
	testNode->setValueVariant(vdoubleA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vdoubleB));
	CHECK(vdoubleA == vdoubleB);

	// Check bool
	bool boolA = true;
	bool boolB = false;
	testNode->setValueBool(boolA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	boolB = outDocument->getRootNode()->getValueBool();
	CHECK(boolA == boolB);
	boolB = false;
	// Serialize as Variant
	Variant vboolA = boolA;
	Variant vboolB = boolB;
	testNode->setValueVariant(vboolA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vboolB));
	CHECK(vboolA == vboolB);

	// Check string
	std::string stringA = std::string("abcdefghijklm nopqrstuvwxyzAZ<>\"\"\'\\\n/!@#$%^&*()~`_+1234567890:;");
	std::string stringB = "";
	testNode->setValueString(stringA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	stringB = outDocument->getRootNode()->getValueString();
	CHECK(stringA == stringB);
	stringB = "";
	// Serialize as Variant
	Variant vstringA = stringA;
	Variant vstringB = stringB;
	testNode->setValueVariant(vstringA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vstringB));
	CHECK(vstringA == vstringB);

	// Check wstring* and WSStringRef
	std::wstring wstringA = L"UTF-8 string \U00000400 \U0000040A \U00000449 \U00000452 \U00000607 \U00000778";
	std::wstring wstringB = L"";
	testNode->setValueWString(wstringA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	wstringB = outDocument->getRootNode()->getValueWString();
	CHECK(wstringA == wstringB);
	wstringB = L"";
	// Serialize as Variant
	Variant vwstringA = wstringA;
	Variant vwstringB = wstringB;
	testNode->setValueVariant(vwstringA);
	document->writeToStream(&rs);
	outDocument->readFromStream(&rs);
	CHECK(outDocument->getRootNode()->getValueVariant(vwstringB));
	CHECK(vwstringA == vwstringB);

	// Check serializing a lone primitive
	Variant a;
	Variant b;
	a = L"UTF-8 string \U00000400 \U0000040A \U00000449 \U00000452 \U00000607 \U00000778";
	CHECK(serializer.serializeToStream(a, &rs, SerializationFormat::XML));
	CHECK(serializer.deserializeFromStream(b, &rs, SerializationFormat::XML));
	CHECK(a == b);
	std::wstring aa = a.cast<std::wstring>();
	CHECK(wcscmp(aa.c_str(), L"UTF-8 string \U00000400 \U0000040A \U00000449 \U00000452 \U00000607 \U00000778") == 0);
	rs.clear();
}

TEST(Serializer_New_Document_Manipulation_XML)
{
	SerializationHandlerManager handlerManager(definitionManager());
	SerializerNew serializer(&handlerManager);
	auto document = serializer.getDocument(SerializationFormat::XML);

	/*
	Test Document Layout:

	<Rootnode>
	    <MyCoolNode>
	        <Type>
	            "coolNode"
	        </Type>
	        <_goodXM.L-_Data990>
	            "'\"/<<>!??"
	        </_goodXM.L-_Data990>
	    </MyCoolNode>
	    <Jabberwock Type="string">	<!-- Serialized variant -->
	        "`Twas brilig, and the slithy toves\ndid gyre and gimble in the wabe."
	    </Jabberwock>
	    <aaa--aaaaaaaaaa>
	        <_bbbbbbbbb7bbXM.L>
	            7
	        </_bbbbbbbbb7bbXM.L>
	    </aaa--aaaaaaaaaa>
	</Rootnode>
	*/

	// Create the document
	{
		auto root = document->getRootNode();
		root->setName("Rootnode");

		auto coolNode = root->createEmptyChild("MyCoolNode");
		// Name and data validation (testing string literal)
		const char* a = "aaaa";
		CHECK(coolNode->createChildString(a, strlen(a), "coolNode") != nullptr);
		CHECK(coolNode->createChildString("Type", "coolNode") != nullptr);
		CHECK(coolNode->createChildString("1badData", "\'\\\"/<<>!??") == nullptr);
		CHECK(coolNode->createChildString("bad<<Data", "\'\\\"/<<>!??") == nullptr);
		CHECK(coolNode->createChildString("xMlbadData", "\'\\\"/<<>!??") == nullptr);
		CHECK(coolNode->createChildString("_goodXM.L-_Data990", "\'\\\"/<<>!??") != nullptr);

		// Test serializing from a string
		std::string childString = "`Twas brilig, and the slithy toves\ndid gyre and gimble in the wabe.";
		// Test char*/size_t
		const char* stringName = "Jabberwock";
		auto jabberwock = root->createEmptyChild(stringName, 10);
		CHECK(jabberwock != nullptr);
		if (jabberwock != nullptr)
		{
			jabberwock->setValueString(childString);
		}

		// Test string
		std::string containerName("aaa--aaaaaaaaaa");
		auto containerNode = root->createEmptyChild(containerName);
		CHECK(containerNode != nullptr);
		if (containerNode != nullptr)
		{
			// Test creating a node with a vector name
			const char* cn = "_bbbbbbbbb7bbXM.L";
			std::vector<char> containedName(strlen(cn));
			// Place characters into vector
			size_t count = 0;
			size_t total = strlen(cn);
			while (count < total)
			{
				containedName[count] = cn[count];
				++count;
			}

			auto containedNode = containerNode->createChildInt(containedName, 7);
			CHECK(containedNode != nullptr);
		}
	}

	// Serialize it
	ResizingMemoryStream rs;
	CHECK(document->writeToStream(&rs));
	rs.seek(0, std::ios_base::beg);

	// Clear the document and check that it's clear
	document->clear();
	CHECK(strcmp(document->getRootNode()->getName().c_str(), "Root") == 0);
	CHECK(document->getRootNode()->getAllChildren().size() == 0);

	// Test that badly-formed XML doesn't parse successfully
	std::string badXMLString("<rootnode><<othernode>7</othernode></rootnode>");
	ResizingMemoryStream badStream(badXMLString);
	CHECK(!document->readFromStream(&badStream));
	document->clear();

	// Test that it parses the well-formed XML stream
	CHECK(document->readFromStream(&rs));
	rs.seek(0, std::ios_base::beg);

	// Check each node
	{
		auto root = document->getRootNode();
		CHECK(strcmp(root->getName().c_str(), "Rootnode") == 0);

		auto coolNode = root->getChildNode("MyCoolNode");
		CHECK(coolNode != nullptr);
		if (coolNode != nullptr)
		{
			// Name and data validation
			auto coolNodeTypeNode = coolNode->getChildNode("Type");
			CHECK(coolNodeTypeNode != nullptr);
			if (coolNodeTypeNode != nullptr)
			{
				CHECK(strcmp(coolNodeTypeNode->getValueString().c_str(), "coolNode") == 0);
			}

			// These should not have been created successfully
			CHECK(coolNode->getChildNode("badXmL<<>>>Data") == nullptr);
			CHECK(coolNode->getChildNode("1badData") == nullptr);
			CHECK(coolNode->getChildNode("bad<<Data") == nullptr);
			CHECK(coolNode->getChildNode("xMlbadData") == nullptr);

			// This should have been created despite its name
			auto goodXMLNode = coolNode->getChildNode("_goodXM.L-_Data990");
			CHECK(goodXMLNode != nullptr);
			if (goodXMLNode != nullptr)
			{
				CHECK(strcmp(goodXMLNode->getValueString().c_str(), "\'\\\"/<<>!??") == 0);
			}
		}

		auto jabberwock = root->getChildNode("Jabberwock");
		CHECK(jabberwock != nullptr);
		if (jabberwock != nullptr)
		{
			std::string childString = jabberwock->getValueString();
			std::string childType = jabberwock->getType();
			bool sameType = false;
			CHECK(sameType = strcmp(childType.c_str(), document->getPrimitiveNames().stringName) == 0);
			if (sameType)
			{
				CHECK(strcmp(childString.c_str(),
				             "`Twas brilig, and the slithy toves\ndid gyre and gimble in the wabe.") == 0);
			}
		}

		auto containerNode = root->getChildNode("aaa--aaaaaaaaaa");
		CHECK(containerNode != nullptr);
		if (containerNode != nullptr)
		{
			auto containedNode = containerNode->getChildNode("_bbbbbbbbb7bbXM.L");
			CHECK(containedNode != nullptr);
			if (containedNode != nullptr)
			{
				CHECK(containedNode->getValueInt() == 7);
			}
		}
	}
}

TEST(Serializer_New_Handlers_XML)
{
	SerializationHandlerManager handlerManager(definitionManager());
	SerializerNew serializer(&handlerManager);

	// Create and register our handlers
	std::shared_ptr<NSTestBigClassHandler> bigHandler = std::make_shared<NSTestBigClassHandler>();
	handlerManager.registerHandler(bigHandler);
	std::shared_ptr<NSTestSmallClassHandler> smallHandler = std::make_shared<NSTestSmallClassHandler>();
	handlerManager.registerHandler(smallHandler);

	// Create our objects
	NSTestBigClass valuesObject;
	valuesObject.setCondition(true);
	valuesObject.setCount(42);
	valuesObject.setName("CoolObject");
	valuesObject.setPoint(59251.2);
	valuesObject.setString("asdfghjkl");

	NSTestSmallClass& preferenceObject = valuesObject.getChild();
	preferenceObject.setFirstPref("CoolFeatureEnabled = true");
	preferenceObject.setSecondPref("Volume = 27");
	preferenceObject.setThirdPref("Language = Pirate");

	// We'll use these for de/serializing.
	Variant a;
	Variant b;

	a = valuesObject;
	auto document = serializer.getDocument(SerializationFormat::XML);
	auto primNames = document->getPrimitiveNames();
	auto rootNode = document->getRootNode();

	// Serialize the variant into the document.
	CHECK(rootNode->setValueVariant(a));

	// Check that the implementation has written the primitive type names correctly.
	auto nameNode = document->getRootNode()->getChildNode("name");
	CHECK(nameNode != nullptr);
	if (nameNode != nullptr)
	{
		CHECK(strcmp(nameNode->getType().c_str(), primNames.stringName) == 0);
	}

	auto stringNode = document->getRootNode()->getChildNode("string");
	CHECK(stringNode != nullptr);
	if (stringNode != nullptr)
	{
		CHECK(strcmp(stringNode->getType().c_str(), primNames.stringName) == 0);
	}

	auto countNode = document->getRootNode()->getChildNode("count");
	CHECK(countNode != nullptr);
	if (countNode != nullptr)
	{
		CHECK(strcmp(countNode->getType().c_str(), primNames.intName) == 0);
	}

	auto conditionNode = document->getRootNode()->getChildNode("condition");
	CHECK(conditionNode != nullptr);
	if (conditionNode != nullptr)
	{
		CHECK(strcmp(conditionNode->getType().c_str(), primNames.boolName) == 0);
	}

	auto pointNode = document->getRootNode()->getChildNode("point");
	CHECK(pointNode != nullptr);
	if (pointNode != nullptr)
	{
		CHECK(strcmp(pointNode->getType().c_str(), primNames.doubleName) == 0);
	}

	// Deserialize the variant
	CHECK(rootNode->getValueVariant(b));

	// Ensure the deserialized object is identical.
	NSTestBigClass deserializedObject = b.cast<NSTestBigClass>();
	CHECK(deserializedObject == valuesObject);

	// HandlerManager will reset handlers when it goes out of scope, but we can do it here just to be sure.
	bigHandler.reset();
	smallHandler.reset();
}

TEST(Serializer_New_Reflection_Handler_XML)
{
	SerializationHandlerManager handlerManager(definitionManager());
	SerializerNew serializer(&handlerManager);

	// Register definitions
	definitionManager().registerDefinition<TypeClassDefinition<ReflectedTestBaseObject>>();
	definitionManager().registerDefinition<TypeClassDefinition<ReflectedTestChildObject>>();
	definitionManager().registerDefinition<TypeClassDefinition<ReflectedTestMemberObject>>();

	// Create and set up our test object
	// ObjectHandleT<ReflectedTestBaseObject> testObjectBaseHandle = createObject<ReflectedTestBaseObject>();
	ObjectHandleT<ReflectedTestChildObject> testObjectHandle = createObject<ReflectedTestChildObject>();
	auto testObject = testObjectHandle.get();
	testObject->setName(L"MyCoolTestObject \U001000FE 唄");
	testObject->setIntValue(42); // Also sets an internal double value.
	testObject->setStringValue("");
	testObject->setChar('&');
	testObject->setWChar(static_cast<wchar_t>(3871));

	// Set up a reflected member object
	ReflectedTestMemberObject& memberObject = testObject->getMemberObject();
	memberObject.setEnabled(true);
	memberObject.setName("ReallyCoolMemberObject");
	memberObject.setValue(73);

	Variant a;
	ReflectedTestChildObject bb;
	Variant b = bb;
	a = testObject;

	auto document = serializer.getDocument(SerializationFormat::XML);
	auto rootNode = document->getRootNode();

	// Reflected handler will handle all the exposed properties automatically
	CHECK(rootNode->setValueVariant(a));

	CHECK(rootNode->getValueVariant(b));

	ReflectedTestChildObject deserializedObject = b.cast<ReflectedTestChildObject>();

	// Compare - will not work if the ReflectedHandler failed to be called and used VariantStream instead.
	CHECK(deserializedObject.getName() == testObject->getName());
	CHECK(deserializedObject.getStringValue() == testObject->getStringValue());
	CHECK(deserializedObject.getIntValue() == testObject->getIntValue());
	CHECK(deserializedObject.getDoubleValue() == testObject->getDoubleValue());
	CHECK(deserializedObject.getUintValue() == testObject->getUintValue());
	CHECK(deserializedObject.getChar() == testObject->getChar());
	CHECK(deserializedObject.getWChar() == testObject->getWChar());

	// Check member reflected object
	auto deserializedMemberObject = deserializedObject.getMemberObject();
	CHECK(deserializedMemberObject.getName() == memberObject.getName());
	CHECK(deserializedMemberObject.getEnabled() == memberObject.getEnabled());
	CHECK(deserializedMemberObject.getValue() == memberObject.getValue());
}

TEST(Serializer_New_Collection_Handler_XML)
{
	// Some primitive types are currently not registered by default, and using them as keys will cause tests to fail.
	// It is fixed on excal ngt, and these tests can be updated to include more types (eg. bool) once merged.

	// Linear containers of arbitrary data
	const std::vector<int64_t> vectorContainer = { 55, 44, 33, 22, 11 };
	std::deque<double> dequeContainer;
	const std::array<int, 13> arrayContainer = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };

	// Map containers of more arbitrary data
	std::map<std::string, std::vector<double>> mapContainerContainer;
	std::unordered_map<int64_t, double> umapContainer;
	std::multimap<std::string, char> mmapContainer;
	std::unordered_multimap<char, double> ummapContainer;

	// Set up deque container
	for (int i = 0; i < 100; ++i)
	{
		if (i % 2 == 0)
		{
			dequeContainer.push_back(double(i));
		}
		else
		{
			dequeContainer.push_front(double(i));
		}
	}

	// Set up map container of vector containers
	std::string strings[] = { "Container1", "Container2", "Container3", "Container4", "Container5" };
	int count = 1;
	for (std::string s : strings)
	{
		std::vector<double> v;
		for (int i = 0; i < 10; ++i)
		{
			v.push_back(double(i * count));
		}
		mapContainerContainer.emplace(std::make_pair(s, v));
		count *= 2;
	}
	// Unordered map container
	for (int i = 0; i < 42; ++i)
	{
		double d = i * 2;
		umapContainer.insert(std::make_pair(i, d));
	}
	// Multimap container
	for (char c = 0; c < 10; ++c)
	{
		if (c % 2 == 0)
		{
			mmapContainer.insert(std::make_pair("true", c));
		}
		else
		{
			mmapContainer.insert(std::make_pair("false", c));
		}
	}
	// Unordered multimap container
	for (char c = 0; c < 100; ++c)
	{
		ummapContainer.insert(std::make_pair(c, double(100 - c)));
	}

	// Create collections from our containers
	Collection vectorCollection(vectorContainer);
	Collection dequeCollection(dequeContainer);
	Collection arrayCollection(arrayContainer);

	Collection mapContainerCollection(mapContainerContainer);
	Collection umapCollection(umapContainer);
	Collection mmapCollection(mmapContainer);
	Collection ummapCollection(ummapContainer);

	// Create the document and rootnode
	SerializationHandlerManager handlerManager(definitionManager());
	SerializerNew serializer(&handlerManager);

	auto document = serializer.getDocument(SerializationFormat::XML);
	auto rootNode = document->getRootNode();

	// Serialize them
	CHECK(rootNode->createChildVariant("vector", vectorCollection));
	CHECK(rootNode->createChildVariant("deque", dequeCollection));
	CHECK(rootNode->createChildVariant("array", arrayCollection));

	CHECK(rootNode->createChildVariant("mapContainer", mapContainerCollection));
	CHECK(rootNode->createChildVariant("umap", umapCollection));
	CHECK(rootNode->createChildVariant("mmap", mmapCollection));
	CHECK(rootNode->createChildVariant("ummap", ummapCollection));

	// Debug output
	ResizingMemoryStream rs;
	document->writeToStream(&rs);

	// Output containers
	std::vector<int64_t> vectorContainerOut;
	std::deque<double> dequeContainerOut;
	std::array<int, 13> arrayContainerOut;
	std::map<std::string, std::vector<double>> mapContainerContainerOut;
	std::unordered_map<int64_t, double> umapContainerOut;
	std::multimap<std::string, char> mmapContainerOut;
	std::unordered_multimap<char, double> ummapContainerOut;

	// Output collections
	Variant vectorCollectionOut = vectorContainerOut;
	Variant dequeCollectionOut = dequeContainerOut;
	Variant arrayCollectionOut = arrayContainerOut;

	Variant mapContainerCollectionOut = mapContainerContainerOut;
	Variant umapCollectionOut = umapContainerOut;
	Variant mmapCollectionOut = mmapContainerOut;
	Variant ummapCollectionOut = ummapContainerOut;

	// Deserialize them
	auto vectorNode = rootNode->getChildNode("vector");
	CHECK(vectorNode != nullptr);
	if (vectorNode != nullptr)
	{
		CHECK(vectorNode->getValueVariant(vectorCollectionOut));
	}
	auto dequeNode = rootNode->getChildNode("deque");
	CHECK(dequeNode != nullptr);
	if (dequeNode != nullptr)
	{
		CHECK(dequeNode->getValueVariant(dequeCollectionOut));
	}
	auto arrayNode = rootNode->getChildNode("array");
	CHECK(arrayNode != nullptr);
	if (arrayNode != nullptr)
	{
		CHECK(arrayNode->getValueVariant(arrayCollectionOut));
	}

	auto mapContainerNode = rootNode->getChildNode("mapContainer");
	CHECK(mapContainerNode != nullptr);
	if (mapContainerNode != nullptr)
	{
		CHECK(mapContainerNode->getValueVariant(mapContainerCollectionOut));
	}
	auto umapNode = rootNode->getChildNode("umap");
	CHECK(umapNode != nullptr);
	if (umapNode != nullptr)
	{
		CHECK(umapNode->getValueVariant(umapCollectionOut));
	}
	auto mmapNode = rootNode->getChildNode("mmap");
	CHECK(mmapNode != nullptr);
	if (mmapNode != nullptr)
	{
		CHECK(mmapNode->getValueVariant(mmapCollectionOut));
	}
	auto ummapNode = rootNode->getChildNode("ummap");
	CHECK(ummapNode != nullptr);
	if (ummapNode != nullptr)
	{
		CHECK(ummapNode->getValueVariant(ummapCollectionOut));
	}

	// Compare them
	CHECK(vectorContainer == vectorCollectionOut.cast<std::vector<int64_t>>());
	CHECK(dequeContainer == dequeCollectionOut.cast<std::deque<double>>());
	bool arraysEqual = arrayContainer == arrayCollectionOut.cast<std::array<int, 13>>();
	CHECK(arraysEqual);

	bool mEqual = mapContainerContainer == mapContainerCollectionOut.cast<std::map<std::string, std::vector<double>>>();
	CHECK(mEqual);
	bool umEqual = umapContainer == umapCollectionOut.cast<std::unordered_map<int64_t, double>>();
	CHECK(umEqual);
	std::multimap<std::string, char> outMmap = mmapCollectionOut.cast<std::multimap<std::string, char>>();
	bool mmEqual = mmapContainer == outMmap;
	CHECK(mmEqual);
	bool ummEqual = ummapContainer == ummapCollectionOut.cast<std::unordered_multimap<char, double>>();
	CHECK(ummEqual);
}
}