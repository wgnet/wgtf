#ifndef HISTORY_MODEL_HPP
#define HISTORY_MODEL_HPP

#include "core_data_model/collection_model.hpp"

namespace wgt
{
/** Data model for wrapping a History display info in a list model. */
class HistoryModel : public CollectionModel
{
public:
	HistoryModel();
	~HistoryModel();
	virtual AbstractItem* item(int index) const override;
};
} // end namespace wgt
#endif // HISTORY_MODEL_HPP
