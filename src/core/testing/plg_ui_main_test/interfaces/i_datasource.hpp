#ifndef I_DATASOURCE_HPP
#define I_DATASOURCE_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
class TestPolyStruct;

typedef ObjectHandleT<TestPolyStruct> TestPolyStructPtr;

class BinaryBlock;
class TestPage;

class IDataSource
{
public:
	virtual const ObjectHandleT<TestPage>& getTestPage() const = 0;
	virtual const char* description() const = 0;
};

class IDataSourceManager
{
public:
	virtual void init(IComponentContext& contextManager) = 0;
	virtual void fini() = 0;
	virtual IDataSource* openDataSource() = 0;
	virtual void closeDataSource(IDataSource* data) = 0;
	virtual std::shared_ptr<BinaryBlock> getThumbnailImage() = 0;
};
} // end namespace wgt
#endif // I_DATASOURCE_HPP
