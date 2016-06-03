
import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Item{
    id: root
    property var id: "chooseProjectDlg"
    property var title: "Choose a project"
    Component.onCompleted: {
        fileDialogInstance.open();
    }

    FileDialog {
        objectName: "ProjectFileDialog"
        id: fileDialogInstance
        folder: ""
        modality: Qt.WindowModal
        nameFilters: ["NGT Project files(*.ngtprj)"]
        selectedNameFilter: "NGT Project files(*.ngtprj)"
        title: "Selected Project File"
        selectMultiple: false
        selectExisting: true

        onAccepted: {
            setOpenProjectFile(fileUrl);
            closeWindow( id );
        }

        onRejected: {
            setOpenProjectFile("");
            closeWindow( id );
        }
    }
}