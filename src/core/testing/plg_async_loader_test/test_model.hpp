#ifndef LIST_TEST_MODEL_HPP
#define LIST_TEST_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_object.hpp"

#include <memory>

namespace wgt
{
class ListDataModel : public AbstractListModel
{
public:
	ListDataModel();
	~ListDataModel();
	void init(int count);

	virtual AbstractItem* item(int row) const override;

	virtual int index(const AbstractItem* item) const override;

	virtual int rowCount() const override;

	virtual int columnCount() const override;

	virtual std::vector<std::string> roles() const override;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

} // end namespace wgt
#endif
