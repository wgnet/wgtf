#pragma once

#include "core_ui_framework/i_dialog.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <QObject>
#include <QQuickWindow>

class QQmlEngine;
class QString;

namespace wgt
{

class QmlDialog : public QObject, public Implements<IDialog>
{
public:
	QmlDialog(QQmlEngine& engine);
	virtual ~QmlDialog();

	virtual const char* title() const override;
	virtual Result result() const override;
	virtual ObjectHandleT<DialogModel> model() const override;
	virtual void setModel(ObjectHandleT<DialogModel> model) override;
	virtual void setModel(ManagedObjectPtr model) override;
	virtual void setModel(const std::nullptr_t&) override;
	virtual void load(const char* resource) override;
	virtual void show(Mode mode) override;
	virtual void raise() override;
	virtual bool isOpen() const override;
	virtual void waitForClose() override;

	Q_INVOKABLE virtual void close(IDialog::Result result) override;

	virtual Connection connectClosedCallback(ClosedCallback callback) override;

	void setParent(QObject* parent);

public slots:
	void error(QQuickWindow::SceneGraphError error, const QString& message);
	void reload(const QString& url);

protected:
	QQmlEngine& engine() const;
	bool eventFilter(QObject* object, QEvent* event) override;

private:
	Q_OBJECT;

	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
