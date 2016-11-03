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

    onOpen: {
        var dir = curValue.substring(0, curValue.lastIndexOf('/'));
        fileDialogInstance.folder = Qt.resolvedUrl(dir);
        fileDialogInstance.open()
    }

    onClose: {
        fileDialogInstance.close()
    }

    FileDialog {
        objectName: "FileDialog"
        id: fileDialogInstance
        folder: mainDialog.folder
        modality: mainDialog.modality
        nameFilters: mainDialog.nameFilters
        selectedNameFilter: mainDialog.selectedNameFilter
        title: mainDialog.title

        onAccepted: {
            mainDialog.accepted(fileUrl)
        }

        onRejected: {
            mainDialog.rejected()
        }
    }
}
