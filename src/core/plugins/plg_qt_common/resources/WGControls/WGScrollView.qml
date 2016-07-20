import QtQuick 2.5
import QtQuick.Controls 1.3

/*!
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
    style: WGScrollViewStyle {}
}
