#ifndef QT_UI_WORKER_HPP
#define QT_UI_WORKER_HPP

#include <QObject>
#include <functional>

struct QtUIFunctionWrapper
{
	QtUIFunctionWrapper(std::function<void()> func) : func_(func)
	{
	}
	std::function<void()> func_;
};

class QtUIWorker : public QObject
{
	Q_OBJECT

public slots:
	void doJob(QtUIFunctionWrapper* wrappedFunc);
};

#endif // QT_UI_WORKER_HPP