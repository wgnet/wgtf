#ifndef TEST_OBJECTS2_HPP
#define TEST_OBJECTS2_HPP
#include "pch.hpp"

#include "core_command_system/command.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_variant/collection.hpp"
#include "wg_types/binary_block.hpp"
#include <vector>

#include "test_command_system_fixture.hpp"
#include <memory>

namespace wgt
{
//------------------------------------------------------------------------------
class TestCommandObject
{
	DECLARE_REFLECTED

public:
	void setCounter(const int& value)
	{
		counter_ = value;
	}
	void getCounter(int* value) const
	{
		*value = counter_;
	}

	void setText(const std::string& value)
	{
		text_ = value;
	}
	const std::string& getText() const
	{
		return text_;
	}

	void incrementCounter()
	{
		++counter_;
	}
	void undo(Variant params, Variant variant)
	{
		--counter_;
	}
	void redo(Variant params, Variant variant)
	{
		++counter_;
	}

	TestCommandObject();
	bool operator==(const TestCommandObject& tdo) const;
	bool operator!=(const TestCommandObject& tdo) const;

	void initialise(int value);

public:
	int counter_;

	std::string text_;

	// PropertyType::String
	std::string string_;
	std::vector<std::string> strings_;

	// PropertyType::WString
	std::wstring wstring_;
	std::vector<std::wstring> wstrings_;

	// PropertyType::Boolean
	bool boolean_;
	std::vector<bool> booleans_;

	// PropertyType::UInt32
	uint32_t uint32_;
	std::vector<uint32_t> uint32s_;

	// PropertyType::Int32
	int32_t int32_;
	std::vector<int32_t> int32s_;

	// PropertyType::UInt64
	uint64_t uint64_;
	std::vector<uint64_t> uint64s_;

	// PropertyType::Float
	float float_;
	std::vector<float> floats_;

	// PropertyType::Raw_Data,
	std::shared_ptr<BinaryBlock> binary_;
	std::vector<std::shared_ptr<BinaryBlock>> binaries_;
};

//------------------------------------------------------------------------------
class TestCommandFixture : public TestCommandSystemFixture
{
public:
	TestCommandFixture();
	~TestCommandFixture();

	void fillValuesWithNumbers(Collection& values);

public:
	IClassDefinition* klass_;
	std::vector<std::unique_ptr<Command>> commands_;
};

//------------------------------------------------------------------------------
class TestThreadCommand : public Command
{
public:
	// This command will simply sleep for 25ms on either the UI or Command threads
	TestThreadCommand(CommandThreadAffinity threadAffinity);

	const char* getId() const
	{
		return id_.c_str();
	}
	Variant execute(const ObjectHandle& arguments) const;

	CommandThreadAffinity threadAffinity() const
	{
		return threadAffinity_;
	}

	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override
	{
		return nullptr;
	}
		
	static std::string generateId(CommandThreadAffinity threadAffinity);

private:
	std::string id_;
	CommandThreadAffinity threadAffinity_;
};

//------------------------------------------------------------------------------
class TestCompoundCommand : public Command
{
public:
	// This command will simply sleep for 25ms and recursively call another TestCompoundCommand.
	// Commands recursively executed will all run on the same thread.
	TestCompoundCommand(int depth, CommandThreadAffinity threadAffinity);

	const char* getId() const
	{
		return id_.c_str();
	}
	virtual Variant execute(const ObjectHandle& arguments) const override;

	CommandThreadAffinity threadAffinity() const
	{
		return threadAffinity_;
	}

	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override
	{
		return nullptr;
	}

	static std::string generateId(int depth, CommandThreadAffinity threadAffinity);

private:
	std::string id_;
	int depth_;
	CommandThreadAffinity threadAffinity_;
};

//------------------------------------------------------------------------------
class TestAlternatingCompoundCommand : public Command
{
public:
	// This command will simply sleep for 25ms and recursively call multiple other TestAlternatingCompoundCommand.
	// Each command recursively called will be executed on alternatively on the UI and Command threads
	TestAlternatingCompoundCommand(int depth, CommandThreadAffinity threadAffinity);

	const char* getId() const
	{
		return id_.c_str();
	}
	virtual Variant execute(const ObjectHandle& arguments) const override;

	CommandThreadAffinity threadAffinity() const
	{
		return threadAffinity_;
	}

	ManagedObjectPtr copyArguments(const ObjectHandle& arguments) const override
	{
		return nullptr;
	}

	static std::string generateId(int depth, CommandThreadAffinity threadAffinity);

private:
	std::string id_;
	int depth_;
	CommandThreadAffinity threadAffinity_;
};
} // end namespace wgt
#endif // TEST_OBJECTS2_HPP
