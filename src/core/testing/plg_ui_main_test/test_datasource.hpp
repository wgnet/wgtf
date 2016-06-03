#ifndef TEST_DATASOURCE_HPP
#define TEST_DATASOURCE_HPP 

#include <unordered_map>
#include "interfaces/i_datasource.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_object_manager.hpp"

namespace wgt
{
class TestDataSource;

class TestDataSourceManager : public Implements< IDataSourceManager >
	, public IObjectManagerListener
{
public:
	TestDataSourceManager() : id_(0) {}
	virtual ~TestDataSourceManager() {}

	virtual void init( IComponentContext & contextManager ) override;
	virtual void fini() override;
	virtual IDataSource* openDataSource() override;
	virtual void closeDataSource( IDataSource* data ) override;
	virtual std::shared_ptr< BinaryBlock > getThumbnailImage() override;

private:
	// IObjectManagerListener
	void onObjectRegistered(const ObjectHandle & pObj);
	void onObjectDeregistered(const ObjectHandle & pObj);

	typedef std::vector< std::pair< int, std::unique_ptr<TestDataSource> > > DataSources;
	DataSources sources_;
	int id_;
	IComponentContext* contextManager_;
	std::unordered_map<std::string, ObjectHandle > loadedObj_;
};

class TestDataSource
	: public IDataSource
{
public:
	TestDataSource( TestDataSourceManager& dataSrcMgr, int id_ );
	virtual ~TestDataSource();

	void init( IComponentContext & contextManager, int id );
	void fini( IComponentContext & contextManager, int id );

	// IDataSource
	const ObjectHandleT< TestPage > & getTestPage() const;
	const ObjectHandleT< TestPage2 > & getTestPage2() const;
	virtual const char* description() const override;

	void setPolyStructObj( const TestPolyStructPtr&  polyStruct );
	const TestPolyStructPtr & getPolyStructObj() const;

private:

    TestDataSourceManager & dataSrcMgr_;
	std::string testPageId_;
	std::string testPageId2_;
	std::string description_;
	ObjectHandleT< TestPage > testPage_;
	ObjectHandleT< TestPage2 > testPage2_;
	
};
} // end namespace wgt
#endif // TEST_DATASOURCE_HPP
