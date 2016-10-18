#include "hello_interface_impl.hpp"
#include "core_reflection/object_handle.hpp"

namespace wgt
{
    HelloInterfaceImpl::HelloInterfaceImpl( IComponentContext & context )
        : Depends( context )
    {
    }

    void HelloInterfaceImpl::display( )
    {
        auto viewCreator = this->get< IViewCreator >();

        helloView_ = viewCreator->createView(
            "WGHelloInterface/HelloInterface.qml",
            ObjectHandle() );
    }

    void HelloInterfaceImpl::remove()
    {
        auto uiApplication = this->get< IUIApplication >();
        if (uiApplication == nullptr)
        {
            return;
        }

        if (helloView_.valid())
        {
            auto view = helloView_.get();
            uiApplication->removeView( *view );
            view = nullptr;
        }
    }
}