#ifndef QT_DEFAULT_SPACING_HPP
#define QT_DEFAULT_SPACING_HPP

#include <QQuickItem>

namespace wgt
{
class QtDefaultSpacing : public QQuickItem
{
	Q_OBJECT

public:
	QtDefaultSpacing()
	{
		minimumRowHeight_ = 22;
		rowSpacing_ = 3;
		leftMargin_ = 10;
		rightMargin_ = 10;
		scrollBarSize_ = 5;
		topBottomMargin_ = 3;
		separatorWidth_ = 2;
		childIndentation_ = 0;
		minimumPanelWidth_ = 400;
		standardBorderSize_ = 1;
		doubleBorderSize_ = standardBorderSize_ * 2;
		standardRadius_ = 4;
		halfRadius_ = standardRadius_ / 2;
		standardMargin_ = standardBorderSize_ + standardRadius_;
		doubleMargin_ = standardMargin_ * 2;
		labelColumnWidth_ = 0;
	}

	Q_PROPERTY(int minimumRowHeight MEMBER minimumRowHeight_ CONSTANT)
	Q_PROPERTY(int rowSpacing MEMBER rowSpacing_ CONSTANT)
	Q_PROPERTY(int leftMargin MEMBER leftMargin_ CONSTANT)
	Q_PROPERTY(int rightMargin MEMBER rightMargin_ CONSTANT)
	Q_PROPERTY(int topBottomMargin MEMBER topBottomMargin_ CONSTANT)
	Q_PROPERTY(int scrollBarSize MEMBER scrollBarSize_ CONSTANT)
	Q_PROPERTY(int separatorWidth MEMBER separatorWidth_ CONSTANT)
	Q_PROPERTY(int childIndentation MEMBER childIndentation_ CONSTANT)
	Q_PROPERTY(int minimumPanelWidth MEMBER minimumPanelWidth_ CONSTANT)
	Q_PROPERTY(int standardBorderSize MEMBER standardBorderSize_ CONSTANT)
	Q_PROPERTY(int doubleBorderSize MEMBER doubleBorderSize_ CONSTANT)
	Q_PROPERTY(int standardRadius MEMBER standardRadius_ CONSTANT)
	Q_PROPERTY(int halfRadius MEMBER halfRadius_ CONSTANT)
	Q_PROPERTY(int standardMargin MEMBER standardMargin_ CONSTANT)
	Q_PROPERTY(int doubleMargin MEMBER doubleMargin_ CONSTANT)
	Q_PROPERTY(int labelColumnWidth MEMBER labelColumnWidth_ NOTIFY labelColumnWidthChanged)

signals:
	void labelColumnWidthChanged();

private:
	int minimumRowHeight_;
	int rowSpacing_;
	int leftMargin_;
	int rightMargin_;
	int topBottomMargin_;
	int scrollBarSize_;
	int separatorWidth_;
	int childIndentation_;
	int minimumPanelWidth_;
	int standardBorderSize_;
	int doubleBorderSize_;
	int standardRadius_;
	int halfRadius_;
	int standardMargin_;
	int doubleMargin_;
	int labelColumnWidth_;
};
} // end namespace wgt
#endif
