#ifndef TEST_TREE_MODEL_HPP
#define TEST_TREE_MODEL_HPP

#include "core_data_model/i_tree_model.hpp"

#include <memory>

namespace wgt
{
class TestTreeModel: public ITreeModel
{
public:
	TestTreeModel();
	TestTreeModel( const TestTreeModel& rhs );
	virtual ~TestTreeModel();

	TestTreeModel& operator=( const TestTreeModel& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* parent ) const override;
	virtual size_t size( const IItem* parent ) const override;
	virtual int columnCount() const override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
