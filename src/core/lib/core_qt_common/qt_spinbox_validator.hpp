#pragma once

#include <QtGui/qvalidator.h>
#include <QtQml/qqml.h>
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
class WGSpinBoxValidator : public QValidator, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)
	Q_PROPERTY(QString text READ text NOTIFY textChanged)
	Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
	Q_PROPERTY(qreal minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
	Q_PROPERTY(qreal maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
	Q_PROPERTY(int decimals READ decimals WRITE setDecimals NOTIFY decimalsChanged)
	Q_PROPERTY(qreal stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged)
	Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)
	Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)
	DECLARE_QT_MEMORY_HANDLER

public:
	explicit WGSpinBoxValidator(QObject* parent = 0);
	virtual ~WGSpinBoxValidator();

	QString text() const;

	QVariant value() const;
	void setValue(QVariant value);

	qreal minimumValue() const;
	void setMinimumValue(qreal min);

	qreal maximumValue() const;
	void setMaximumValue(qreal max);

	int decimals() const;
	void setDecimals(int decimals);

	qreal stepSize() const;
	void setStepSize(qreal step);

	QString prefix() const;
	void setPrefix(const QString& prefix);

	QString suffix() const;
	void setSuffix(const QString& suffix);

	void fixup(QString& input) const;
	State validate(QString& input, int& pos) const;

	void classBegin()
	{
	}
	void componentComplete();

public Q_SLOTS:
	void increment();
	void decrement();

Q_SIGNALS:
	void valueChanged();
	void minimumValueChanged();
	void maximumValueChanged();
	void decimalsChanged();
	void stepSizeChanged();
	void prefixChanged();
	void suffixChanged();
	void textChanged();

protected:
	QVariant variantFromText(const QString& input, bool& ok) const;
	QString textFromValue(const QVariant& value) const;
	QVariant boundValue(const QVariant& value) const;
	bool compareValue(const QVariant& value) const;

private:
	QVariant value_;
	qreal step_;
	int	decimals_ = 3;
	QString prefix_;
	QString suffix_;
	bool initialized_;
	QDoubleValidator validator_;

	Q_DISABLE_COPY(WGSpinBoxValidator)
};
}

QML_DECLARE_TYPE(wgt::WGSpinBoxValidator)
