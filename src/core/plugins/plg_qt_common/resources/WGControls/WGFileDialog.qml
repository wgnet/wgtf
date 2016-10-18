import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import WGControls 1.0

/*!
 \ingroup wgcontrols
*/
Item {
    id: mainDialog
    objectName: "WGFileDialog"
    WGComponent { type: "WGFileDialog" }

    /*! The folder the fileDialog will open to by default.

      The default value is determined by the control that opens it.
    */
    property url folder

    /*! Whether the dialog should be shown modal with respect to the window containing the dialog's parent Item,
        modal with respect to the whole application, or non-modal.

      The default value is determined by the control that opens it.
    */
    property var modality

    /*! This array contains a list of filename Filters in the format:
      \code{.js}
      description (*.extension1 *.extension2 ...)
      \endcode
      All files can be selected using (*)

      The default value is determined by the control that opens it.
    */
    property var nameFilters

    /*! This currently selected filename filter

      The default value is determined by the control that opens it.
    */
    property string selectedNameFilter

    /*! The heading of the dialog box.

      The default value is determined by the control that opens it.
    */
    property string title

    /*! Opens (displays) the dialog box.

    */
    signal open(int dWidth, int dHeight, var curValue)

    /*! Closes (hides) the dialog box.

    */
    signal close()

    /*! fires when a file has been selected and returns the selected file.

    */
    signal accepted(var selectedValue)

    /*! fires when the dialog has been cancelled without selecting a file.

    */
    signal rejected()
}
