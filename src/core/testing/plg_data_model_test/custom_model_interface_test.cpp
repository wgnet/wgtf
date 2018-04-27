#include "custom_model_interface_test.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework//interfaces/i_view_creator.hpp"
#include "core_data_model/file_system/file_system_model.hpp"
#include "core_serialization/i_file_system.hpp"

namespace wgt
{
class ICustomModelInterface
{
	DECLARE_REFLECTED

public:
	ICustomModelInterface()
	{
	}

protected:
	ICustomModelInterface(int numeric, std::string string) : numeric_(numeric), string_(string)
	{
	}

	void incrementNumeric(double value)
	{
		numeric_ += static_cast<int>(value);
	}

	void undoIncrementNumeric(Variant params, Variant result)
	{
		double value = params.cast<ReflectedMethodParameters>()[0].cast<double>();
		numeric_ -= static_cast<int>(value);
	}

	void redoIncrementNumeric(Variant params, Variant result)
	{
		double value = params.cast<ReflectedMethodParameters>()[0].cast<double>();
		numeric_ += static_cast<int>(value);
	}

private:
	int numeric_;
	std::string string_;
};

class CustomModelImplementation1 : public ICustomModelInterface
{
public:
	CustomModelImplementation1() : ICustomModelInterface(1, "Implementation 1")
	{
	}
};

class CustomModelImplementation2 : public ICustomModelInterface
{
public:
	CustomModelImplementation2() : ICustomModelInterface(2, "Implementation 2")
	{
	}
};

class CustomModelImplementation3 : public ICustomModelInterface
{
public:
	CustomModelImplementation3() : ICustomModelInterface(3, "Implementation 3")
	{
	}
};

struct TestChildObject
{
	TestChildObject() : number_(0)
	{
	}

	int getNumber() const
	{
		return number_;
	}

	void setNumber(const int& value)
	{
		number_ = value;
	}

	int number_;
};

struct TestParentObject
{
	TestParentObject() : number_(0)
	{
	}

	TestChildObject getChild() const
	{
		return child_;
	}

	void setChild(const TestChildObject& value)
	{
		child_ = value;
	}

	TestChildObject child_;
	int number_;
};

class TestFixture
{
	DECLARE_REFLECTED

public:
	enum class Enum
	{
		Value1,
		Value2,
		Value3
	};
	void init(IDefinitionManager* defManager, IFileSystem* fileSystem)
	{
		auto def = defManager->getDefinition<ICustomModelInterface>();
		implementation1_ = std::unique_ptr<ICustomModelInterface>(new CustomModelImplementation1);
		implementation2_ = std::unique_ptr<ICustomModelInterface>(new CustomModelImplementation2);
		implementation3_ = std::unique_ptr<ICustomModelInterface>(new CustomModelImplementation3);
		fileSystemModel_ = std::unique_ptr<AbstractTreeModel>(new FileSystemModel(*fileSystem, "/"));
		parentObject_ = std::make_unique<TestParentObject>();
		enum1_ = 0;
		enum2_ = 3;
		enum3_ = Enum::Value1;
	}

	ICustomModelInterface* implementation1() const
	{
		return implementation1_.get();
	}

	ICustomModelInterface* implementation2() const
	{
		return implementation2_.get();
	}

	ICustomModelInterface* implementation3() const
	{
		return implementation3_.get();
	}

	AbstractTreeModel* fileSystemModel() const
	{
		return fileSystemModel_.get();
	}

	TestParentObject* parentObject() const
	{
		return parentObject_.get();
	}

	void enumValues(std::map<int, Variant>* o_enumMap) const
	{
		o_enumMap->clear();
		(*o_enumMap)[3] = L"Select3";
		(*o_enumMap)[4] = L"Select4";
		(*o_enumMap)[5] = L"Select5";
	}

private:
	std::unique_ptr<ICustomModelInterface> implementation1_;
	std::unique_ptr<ICustomModelInterface> implementation2_;
	std::unique_ptr<ICustomModelInterface> implementation3_;
	std::unique_ptr<AbstractTreeModel> fileSystemModel_;
	std::unique_ptr<TestParentObject> parentObject_;
	int enum1_;
	int enum2_;
	Enum enum3_;
};

BEGIN_EXPOSE(ICustomModelInterface, MetaNone())
EXPOSE("numeric", numeric_, MetaNone())
EXPOSE("string", string_, MetaNone())
EXPOSE_METHOD("incrementNumeric", incrementNumeric, undoIncrementNumeric, redoIncrementNumeric)
END_EXPOSE()

BEGIN_EXPOSE(TestChildObject, MetaNone())
EXPOSE("number", getNumber, setNumber, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE(TestParentObject, MetaNone())
EXPOSE("child", getChild, setChild, MetaNone())
EXPOSE("number", number_, MetaNone())
END_EXPOSE()

BEGIN_EXPOSE_ENUM(TestFixture::Enum)
	{TestFixture::Enum::Value1, "Value1"},
	{TestFixture::Enum::Value2, "Value2"},
	{TestFixture::Enum::Value3, "Value3"},
END_EXPOSE()

BEGIN_EXPOSE(TestFixture, MetaNone())
EXPOSE("Implementation1", implementation1, MetaNone())
EXPOSE("Implementation2", implementation2, MetaNone())
EXPOSE("Implementation3", implementation3, MetaNone())
EXPOSE("fileSystemModel", fileSystemModel, MetaNone())
EXPOSE("enum1", enum1_, MetaEnum(L"Select0=0|Select1|Select2"))
EXPOSE("enum2", enum2_, MetaEnumFunc(enumValues))
EXPOSE("enum3", enum3_, MetaEnum<TestFixture::Enum>())
EXPOSE("parentObject", parentObject, MetaNone())
END_EXPOSE()

CustomModelInterfaceTest::~CustomModelInterfaceTest()
{
}

void CustomModelInterfaceTest::initialise()
{
	auto defManager = get<IDefinitionManager>();
	auto fileSystem = get<IFileSystem>();
	if (defManager == nullptr || fileSystem == nullptr)
	{
		return;
	}

	defManager->registerDefinition<TypeClassDefinition<TestChildObject>>();
	defManager->registerDefinition<TypeClassDefinition<TestParentObject>>();
	defManager->registerDefinition<TypeClassDefinition<ICustomModelInterface>>();
	defManager->registerDefinition<TypeClassDefinition<TestFixture>>();

	testFixture_ = ManagedObject<TestFixture>::make();
    testFixture_->init(defManager, fileSystem);

	auto viewCreator = get<IViewCreator>();
	if (viewCreator)
	{
		testView_ = viewCreator->createView("plg_data_model_test/custom_model_interface_test_panel.qml", testFixture_.getHandleT());
	}
}

void CustomModelInterfaceTest::fini()
{
	auto uiApplication = get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (testView_.valid())
	{
		auto view = testView_.get();
		uiApplication->removeView(*view);
		view = nullptr;
	}

    testFixture_ = nullptr;
}
} // end namespace wgt
