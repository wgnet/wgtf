#ifndef WG_FILTERED_TREE_MODEL_HPP
#define WG_FILTERED_TREE_MODEL_HPP

#include "core_qt_common/qt_new_handler.hpp"
#include "core_qt_common/models/wg_tree_model.hpp"
#include "core_qt_common/helpers/wg_filter.hpp"
#include <memory>

namespace wgt
{
class WGFilteredTreeModel : public WGTreeModel
{
	Q_OBJECT

	Q_PROPERTY(QObject* filter READ getFilter WRITE setFilter NOTIFY filterChanged)

	DECLARE_QT_MEMORY_HANDLER

public:
	WGFilteredTreeModel();
	virtual ~WGFilteredTreeModel();

protected:
	virtual ITreeModel* getModel() const override;

private:
	void onSourceChanged();
	QObject* getFilter() const;
	void setFilter(QObject* filter);

signals:
	void filterChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // WG_FILTERED_TREE_MODEL_HPP
