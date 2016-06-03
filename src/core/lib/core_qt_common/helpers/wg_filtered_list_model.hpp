#ifndef WG_FILTERED_LIST_MODEL_HPP
#define WG_FILTERED_LIST_MODEL_HPP

#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/models/wg_list_model.hpp"
#include "core_qt_common/helpers/wg_filter.hpp"
#include <memory>

namespace wgt
{
class WGFilteredListModel : public WGListModel
{
	Q_OBJECT

	Q_PROPERTY( QObject* filter
				READ getFilter
				WRITE setFilter
				NOTIFY filterChanged )

	Q_PROPERTY( bool isFiltering
				READ getIsFiltering )

	DECLARE_QT_MEMORY_HANDLER

public:
	WGFilteredListModel();
	virtual ~WGFilteredListModel();

protected:
	virtual IListModel * getModel() const override;

private:
	void onSourceChanged();
	QObject * getFilter() const;
	void setFilter( QObject * filter );
	bool getIsFiltering() const;

signals:
	void filterChanged();
	void filteringBegin();
	void filteringEnd();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // WG_FILTERED_LIST_MODEL_HPP
