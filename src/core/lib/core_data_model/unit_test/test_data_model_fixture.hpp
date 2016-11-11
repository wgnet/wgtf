#ifndef TEST_DATA_MODEL_FIXTURE_HPP
#define TEST_DATA_MODEL_FIXTURE_HPP

#include <memory>

namespace wgt
{
class IObjectManager;
class IDefinitionManager;
class ObjectManager;
class DefinitionManager;

class TestDataModelFixture
{
public:
	TestDataModelFixture();
	virtual ~TestDataModelFixture();
	IObjectManager* getObjectManager() const;
	IDefinitionManager* getDefinitionManager() const;

private:
	std::unique_ptr<ObjectManager> objectManager_;
	std::unique_ptr<DefinitionManager> definitionManager_;
};
} // end namespace wgt
#endif // TEST_DATA_MODEL_FIXTURE_HPP
