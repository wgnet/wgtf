#ifndef WG_TOKENIZED_STRING_FILTER_HPP
#define WG_TOKENIZED_STRING_FILTER_HPP

#include "../qt_new_handler.hpp"
#include "wg_filter.hpp"
#include <memory>

class QString;
class QVariant;

namespace wgt
{
/**
 *	WGTokenizedStringFilter
 *	A tokenized string filter wrapper around a TokenizedStringFilter implementation.
 */
class WGTokenizedStringFilter : public WGFilter
{
	Q_OBJECT

	Q_PROPERTY(QString filterText READ getFilterText WRITE setFilterText NOTIFY filterTextChanged)

	Q_PROPERTY(QString splitterChar READ getSplitterChar WRITE setSplitterChar NOTIFY splitterCharChanged)

	DECLARE_QT_MEMORY_HANDLER

public:
	WGTokenizedStringFilter();
	virtual ~WGTokenizedStringFilter();

	virtual IItemFilter* getFilter() const override;

protected:
	virtual void updateInternalItemRole() override;

private:
	QString getFilterText() const;
	void setFilterText(const QString& filterText);

	QString getSplitterChar() const;
	void setSplitterChar(const QString& splitter);

signals:
	void filterTextChanged();
	void splitterCharChanged();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // WG_TOKENIZED_STRING_FILTER_HPP
