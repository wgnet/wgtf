#include "pch.hpp"
#include "test_framework.hpp"

namespace wgt
{

struct TestFramework::Impl
{
	Impl()
		: objectManager_(std::make_unique<TestObjectManager>())
		, metaTypeCreator_(std::make_unique<TestMetaTypeCreator>())
		, definitionManager_(std::make_unique<DefinitionManager>(*objectManager_))
	{
		objectManager_->init(definitionManager_.get());
		ReflectionShared::initContext(*getGlobalContext());
		ReflectionShared::initDefinitionManager(*definitionManager_);
		defManagerHolder_ =
			registerInterface< IDefinitionManager >(definitionManager_.get());
	}

	~Impl()
	{
		deregisterInterface(defManagerHolder_.get());
	}

	std::unique_ptr<TestObjectManager> objectManager_;
	std::unique_ptr<TestMetaTypeCreator> metaTypeCreator_;
	std::unique_ptr<DefinitionManager> definitionManager_;

	InterfacePtr defManagerHolder_;
};


//------------------------------------------------------------------------------
TestFramework::TestFramework()
	: impl_( std::make_unique<Impl>())
{
}

//------------------------------------------------------------------------------
TestFramework::~TestFramework()
{
}

//------------------------------------------------------------------------------
IObjectManager& TestFramework::getObjectManager() const
{
    return *impl_->objectManager_;
}
//------------------------------------------------------------------------------
IDefinitionManager& TestFramework::getDefinitionManager() const
{
    return *impl_->definitionManager_;
}

} //end namespace wgt