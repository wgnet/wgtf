import QtQuick 2.5
import QtQuick.Controls 1.4

import WGControls.Private 2.0
import WGControls.Styles 2.0

/*!
 \ingroup wgcontrols
 \brief Scrollable panel with styled Scroll bar.

\code{.js}
WGScrollView {
    anchors.fill:parent
    WGListView
    {
        //Can be any Object or Flickable
    }
}
\endcode
*/

ScrollView {
    objectName: "WGScrollView"
    WGComponent { type: "WGScrollView20" }
    
    style: WGScrollViewStyle {}
}
