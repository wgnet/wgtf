#include "demo_objects_fix_mixin.hpp"

#include <QQuickItem>
#include <QQuickWindow>

namespace wgt
{
DemoObjectsFixMixIn::DemoObjectsFixMixIn()
	: valid_(true)
{
}

void DemoObjectsFixMixIn::componentComplete(Variant userData)
{
	ObjectHandle handle;
	bool isOk = userData.tryCast(handle);
	assert(isOk);
	auto control = handle.getBase< QObject >();
	auto item = qobject_cast<QQuickItem *>(control);
	item->connect(item, &QQuickItem::windowChanged, [ this, item ] (QQuickWindow *window )
	{
		item->connect( window, &QQuickWindow::sceneGraphInvalidated, [this]
		{
			valid_ = false;
		});
	});
	control->connect(item->window(), &QQuickWindow::sceneGraphInvalidated, [this]
	{
		valid_ = false;
	});
}

bool DemoObjectsFixMixIn::shouldInitializeGL()
{
	auto valid = valid_;
	valid_ = true;
	return valid;
}
} // end namespace wgt
