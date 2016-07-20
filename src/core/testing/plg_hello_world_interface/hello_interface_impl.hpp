#pragma once
#ifndef HELLO_INTERFACE_IMPL_H
#define HELLO_INTERFACE_IMPL_H

#include "../interfaces_test/hello_interface.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include <memory>

namespace wgt
{

 /**
 * HelloInterfaceImpl
 *
 * Plugins are driven by their interfaces, which are versioned. HelloInterface
 * is one such interface, which is at version 1 at the moment. It implements
 * the display() and remove() functions, which will create our HelloInterfaceImpl.
 */
class HelloInterfaceImpl
    : public Implements< HelloInterface > // Always implement latest version
    , Depends< IUIFramework, IUIApplication, IViewCreator >
{

public:

    HelloInterfaceImpl( IComponentContext & context );
    HelloInterfaceImpl();
     
    virtual void display() override;
    virtual void remove() override;
 
private:
    
    wg_future<std::unique_ptr< IView >> helloView_;
};

} // end namespace wgt

#endif // HELLO_INTERFACE_IMPL_H