import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2


/*!
  \ingroup wgcontrols
 \brief A WG specific file dialog
*/

WGFileDialog {
    id: mainDialog
    objectName: "WGNativeFileDialog"
    WGComponent { type: "WGNativeFileDialog" }

    function open(dWidth, dHeight, curValue) {
        var dir = curValue.substring(0, curValue.lastIndexOf('/'));
        fileDialogInstance.folder = Qt.resolvedUrl(dir);
        fileDialogInstance.open()
    }

    function close() {
        fileDialogInstance.close()
    }

    FileDialog {
        objectName: "FileDialog"
        id: fileDialogInstance
        folder: mainDialog.folder
        modality: mainDialog.modality
        nameFilters: typeof mainDialog.nameFilters == "undefined" ? ["All files (*)"] : mainDialog.nameFilters
        selectedNameFilter: mainDialog.selectedNameFilter
        title: mainDialog.title

        onAccepted: {
            mainDialog.accepted(fileUrl)
        }

        onRejected: {
            mainDialog.rejected()
        }

        onVisibleChanged: {
            if (!visible) {
                mainDialog.closed();
            }
            else {
                mainDialog.opened();
            }
        }
    }
}
