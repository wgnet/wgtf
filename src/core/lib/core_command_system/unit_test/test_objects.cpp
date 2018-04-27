#include "pch.hpp"

#include "core_command_system/i_command_manager.hpp"
#include "core_common/assert.hpp"
#include "core_reflection/interfaces/i_class_definition.hpp"

#include "core_reflection/property_accessor.hpp"
#include "core_reflection/definition_manager.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

#include "wg_types/binary_block.hpp"

#include "test_objects.hpp"

#include <thread>

namespace wgt
{

TestCommandFixture::TestCommandFixture()
{
	IDefinitionManager& definitionManager = getDefinitionManager();
	klass_ = definitionManager.getDefinition<TestCommandObject>();

	// Register all the required test commands
	commands_.emplace_back(new TestThreadCommand(CommandThreadAffinity::UI_THREAD));
	commands_.emplace_back(new TestThreadCommand(CommandThreadAffinity::COMMAND_THREAD));
	commands_.emplace_back(new TestThreadCommand(CommandThreadAffinity::ANY_THREAD));
	for (auto i = 0; i < 5; ++i)
	{
		commands_.emplace_back(new TestCompoundCommand(i, CommandThreadAffinity::UI_THREAD));
		commands_.emplace_back(new TestCompoundCommand(i, CommandThreadAffinity::COMMAND_THREAD));
		commands_.emplace_back(new TestCompoundCommand(i, CommandThreadAffinity::ANY_THREAD));
		commands_.emplace_back(new TestAlternatingCompoundCommand(i, CommandThreadAffinity::UI_THREAD));
		commands_.emplace_back(new TestAlternatingCompoundCommand(i, CommandThreadAffinity::COMMAND_THREAD));
		commands_.emplace_back(new TestAlternatingCompoundCommand(i, CommandThreadAffinity::ANY_THREAD));
	}

	auto& commandManager = getCommandSystemProvider();
	for (auto& command : commands_)
	{
		commandManager.registerCommand(command.get());
	}
}

TestCommandFixture::~TestCommandFixture()
{
}

void TestCommandFixture::fillValuesWithNumbers(Collection& values)
{
	float increment = 3.25f;
	float value = 1.0f;
	for (int i = 0; i < 5; ++i)
	{
		auto it = values.insert(i);
		it.setValue(value);
		value += increment;
		increment += 3.25f;
	}
}

namespace
{
uint32_t RandomNumber32()
{
	RefObjectId uid(RefObjectId::generate());
	return uid.getA() + uid.getB() + uid.getC() + uid.getD();
}

uint64_t RandomNumber64()
{
	RefObjectId uid(RefObjectId::generate());
	uint64_t a = (uint64_t)uid.getA();
	uint64_t b = (uint64_t)uid.getB();
	uint64_t c = (uint64_t)uid.getC();
	uint64_t d = (uint64_t)uid.getD();
	return (a << 32) + (b << 32) + c + d;
}

std::string RandomString()
{
	std::string random("Random Data: ");
	random += RefObjectId::generate().toString();
	random += RefObjectId::generate().toString();
	random += RefObjectId::generate().toString();
	random += RefObjectId::generate().toString();
	return random;
}
}

//------------------------------------------------------------------------------
TestCommandObject::TestCommandObject() : counter_(0), boolean_(false), uint32_(0), int32_(0), uint64_(0L), float_(0.f)
{
}

//------------------------------------------------------------------------------
void TestCommandObject::initialise(int value)
{
	counter_ = value;

	std::stringstream ss;
	ss << "TestDefinitionObject " << value + 1;
	text_ = ss.str();

	string_ = RefObjectId::generate().toString();
	strings_.push_back(RefObjectId::generate().toString());
	strings_.push_back(RefObjectId::generate().toString());

	std::wstringstream wss;
	wss << L"TestDefinitionObject " << value + 1 << L" (ÔÓ-ÛÒÒÍË)";
	wstring_ = wss.str();
	wstrings_.push_back(wstring_);
	wstrings_.push_back(wstring_);

	boolean_ = value % 2 == 0;
	booleans_.push_back(value % 2 == 0);
	booleans_.push_back(value % 2 != 0);

	uint32_ = value;
	uint32s_.resize(5);
	std::generate(uint32s_.begin(), uint32s_.end(), RandomNumber32);

	int32_ = value;
	int32s_.resize(5);
	std::generate(int32s_.begin(), int32s_.end(), RandomNumber32);

	uint64_ = value;
	uint64s_.resize(5);
	std::generate(uint64s_.begin(), uint64s_.end(), RandomNumber64);

	float_ = (float)value;
	floats_.resize(5);
	std::generate(floats_.begin(), floats_.end(), RandomNumber32);

	std::string randomData(RandomString());
	binary_ = std::make_shared<BinaryBlock>(randomData.c_str(), randomData.size(), false);
	binaries_.push_back(std::make_shared<BinaryBlock>(randomData.c_str(), randomData.size(), false));
	std::string moreRandomData(RandomString());
	binaries_.push_back(std::make_shared<BinaryBlock>(moreRandomData.c_str(), moreRandomData.size(), false));
}

//------------------------------------------------------------------------------
bool TestCommandObject::operator==(const TestCommandObject& tdo) const
{
	if (counter_ != tdo.counter_)
		return false;

	if (text_ != tdo.text_)
		return false;

	if (string_ != tdo.string_ || strings_ != tdo.strings_)
		return false;

	if (wstring_ != tdo.wstring_ || wstrings_ != tdo.wstrings_)
		return false;

	if (boolean_ != tdo.boolean_ || booleans_ != tdo.booleans_)
		return false;

	if (uint32_ != tdo.uint32_ || uint32s_ != tdo.uint32s_)
		return false;

	if (int32_ != tdo.int32_ || int32s_ != tdo.int32s_)
		return false;

	if (uint64_ != tdo.uint64_ || uint64s_ != tdo.uint64s_)
		return false;

	if (float_ != tdo.float_ || floats_ != tdo.floats_)
		return false;

	if (binary_->compare(*tdo.binary_) != 0 || binaries_.size() != tdo.binaries_.size())
		return false;

	for (size_t i = 0; i < binaries_.size(); ++i)
	{
		if (binaries_[i]->compare(*tdo.binaries_[i]) != 0)
			return false;
	}

	return true;
}

//------------------------------------------------------------------------------
bool TestCommandObject::operator!=(const TestCommandObject& tdo) const
{
	return !operator==(tdo);
}

//------------------------------------------------------------------------------
TestThreadCommand::TestThreadCommand(CommandThreadAffinity threadAffinity)
    : id_(generateId(threadAffinity)), threadAffinity_(threadAffinity)
{
}

//------------------------------------------------------------------------------
Variant TestThreadCommand::execute(const ObjectHandle& arguments) const
{
	// TODO assert current thread is expected thread
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	return CommandErrorCode::COMMAND_NO_ERROR;
}

//------------------------------------------------------------------------------
std::string TestThreadCommand::generateId(CommandThreadAffinity threadAffinity)
{
	std::string id = "TestThreadCommand";
	switch (threadAffinity)
	{
	case CommandThreadAffinity::NO_THREAD:
		id += "_NO_THREAD";
		break;

	case CommandThreadAffinity::UI_THREAD:
		id += "_UI_THREAD";
		break;

	case CommandThreadAffinity::COMMAND_THREAD:
		id += "_COMMAND_THREAD";
		break;

	case CommandThreadAffinity::ANY_THREAD:
		id += "_ANY_THREAD";
		break;
	}
	return id;
}

//------------------------------------------------------------------------------
TestCompoundCommand::TestCompoundCommand(int depth, CommandThreadAffinity threadAffinity)
    : id_(generateId(depth, threadAffinity)), depth_(depth), threadAffinity_(threadAffinity)
{
}

//------------------------------------------------------------------------------
Variant TestCompoundCommand::execute(const ObjectHandle& arguments) const
{
	// TODO assert current thread is expected thread
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	if (depth_ == 0)
	{
		return CommandErrorCode::COMMAND_NO_ERROR;
	}

	auto commandManager = getCommandSystemProvider();

	// wait for a recursive command
	auto recursiveCommand =
	commandManager->queueCommand(TestCompoundCommand::generateId(depth_ - 1, threadAffinity_).c_str());
	commandManager->waitForInstance(recursiveCommand);
	TF_ASSERT(recursiveCommand->isComplete());

	// generate root commands and wait for them
	auto rootCommand1 = commandManager->queueCommand(TestCompoundCommand::generateId(0, threadAffinity_).c_str());
	auto rootCommand2 = commandManager->queueCommand(TestCompoundCommand::generateId(0, threadAffinity_).c_str());
	commandManager->waitForInstance(rootCommand1);
	TF_ASSERT(rootCommand1->isComplete());
	auto rootCommand3 = commandManager->queueCommand(TestCompoundCommand::generateId(0, threadAffinity_).c_str());
	commandManager->waitForInstance(rootCommand3);
	TF_ASSERT(rootCommand2->isComplete());
	TF_ASSERT(rootCommand3->isComplete());

	// generate a recursive command and allow the command system to automatically execute on completion of this command
	commandManager->queueCommand(TestCompoundCommand::generateId(depth_ - 1, threadAffinity_).c_str());

	return CommandErrorCode::COMMAND_NO_ERROR;
}

//------------------------------------------------------------------------------
std::string TestCompoundCommand::generateId(int depth, CommandThreadAffinity threadAffinity)
{
	std::string id = "TestCompoundCommand";
	char buffer[8];
	sprintf(buffer, "%d", depth);
	id += "_" + std::string(buffer);
	switch (threadAffinity)
	{
	case CommandThreadAffinity::NO_THREAD:
		id += "_NO_THREAD";
		break;

	case CommandThreadAffinity::UI_THREAD:
		id += "_UI_THREAD";
		break;

	case CommandThreadAffinity::COMMAND_THREAD:
		id += "_COMMAND_THREAD";
		break;

	case CommandThreadAffinity::ANY_THREAD:
		id += "_ANY_THREAD";
		break;
	}
	return id;
}

//------------------------------------------------------------------------------
TestAlternatingCompoundCommand::TestAlternatingCompoundCommand(int depth, CommandThreadAffinity threadAffinity)
    : id_(generateId(depth, threadAffinity)), depth_(depth), threadAffinity_(threadAffinity)
{
}

//------------------------------------------------------------------------------
Variant TestAlternatingCompoundCommand::execute(const ObjectHandle& arguments) const
{
	// TODO assert current thread is expected thread
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	if (depth_ == 0)
	{
		return CommandErrorCode::COMMAND_NO_ERROR;
	}

	auto commandManager = getCommandSystemProvider();

	// queue recursive commands of different threadAffinity
	auto uiThreadCommand = commandManager->queueCommand(
	TestAlternatingCompoundCommand::generateId(depth_ - 1, CommandThreadAffinity::UI_THREAD).c_str());
	auto commandThreadCommand = commandManager->queueCommand(
	TestAlternatingCompoundCommand::generateId(depth_ - 1, CommandThreadAffinity::COMMAND_THREAD).c_str());
	auto anyThreadCommand = commandManager->queueCommand(
	TestAlternatingCompoundCommand::generateId(depth_ - 1, CommandThreadAffinity::ANY_THREAD).c_str());

	return CommandErrorCode::COMMAND_NO_ERROR;
}

//------------------------------------------------------------------------------
std::string TestAlternatingCompoundCommand::generateId(int depth, CommandThreadAffinity threadAffinity)
{
	std::string id = "TestAlternatingCompoundCommand";
	char buffer[8];
	sprintf(buffer, "%d", depth);
	id += "_" + std::string(buffer);
	switch (threadAffinity)
	{
	case CommandThreadAffinity::NO_THREAD:
		id += "_NO_THREAD";
		break;

	case CommandThreadAffinity::UI_THREAD:
		id += "_UI_THREAD";
		break;

	case CommandThreadAffinity::COMMAND_THREAD:
		id += "_COMMAND_THREAD";
		break;

	case CommandThreadAffinity::ANY_THREAD:
		id += "_ANY_THREAD";
		break;
	}
	return id;
}
} // end namespace wgt
