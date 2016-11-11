#ifndef WG_STRING_FILTER_HPP
#define WG_STRING_FILTER_HPP

#include "../qt_new_handler.hpp"
#include "wg_filter.hpp"
#include <memory>

class QString;
class QVariant;

namespace wgt
{
/**
 *	WGStringFilter
 *	A basic string-based filter wrapper around a StringFilter implementation.
 */
class WGStringFilter : public WGFilter
{
	Q_OBJECT

	Q_PROPERTY(QString filterText READ getFilterText WRITE setFilterText NOTIFY filterTextChanged)

	DECLARE_QT_MEMORY_HANDLER

public:
	WGStringFilter();
	virtual ~WGStringFilter();

	virtual IItemFilter* getFilter() const override;

protected:
	virtual void updateInternalItemRole() override;

private:
	QString getFilterText() const;
	void setFilterText(const QString& filterText);

signals:
	void filterTextChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // WG_STRING_FILTER_HPP
