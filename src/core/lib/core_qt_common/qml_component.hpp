#ifndef QML_COMPONENT_HPP
#define QML_COMPONENT_HPP

#include "core_ui_framework/i_component.hpp"

#include <memory>

class QQmlComponent;
class QQmlEngine;

namespace wgt
{
class QmlComponent : public IComponent
{
public:
	QmlComponent(QQmlEngine& qmlEngine);
	virtual ~QmlComponent();

	QQmlComponent* component() const;

private:
	std::unique_ptr<QQmlComponent> qmlComponent_;
};
} // end namespace wgt
#endif
