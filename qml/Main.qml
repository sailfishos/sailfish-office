import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office 1.0
import org.kde.calligra 1.0 as Calligra
import Sailfish.Office.Files 1.0

ApplicationWindow
{
    id: window;

    property Item documentItem
    property QtObject fileListModel

    Connections {
        target: Qt.application
        onActiveChanged: window.allowedOrientations = Qt.application.active ? Orientation.All : pageStack.currentOrientation
    }
    function mimeToIcon(fileMimeType) {
        // TODO: move all graphics to platform theme packages
        switch (fileMimeType) {
        case "application/vnd.oasis.opendocument.spreadsheet":
        case "application/x-kspread":
        case "application/vnd.ms-excel":
        case "text/csv":
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
        case "application/vnd.openxmlformats-officedocument.spreadsheetml.template":
            return "images/icon-m-mime-spreadsheet.png";
        case "application/vnd.oasis.opendocument.presentation":
        case "application/vnd.oasis.opendocument.presentation-template":
        case "application/x-kpresenter":
        case "application/vnd.ms-powerpoint":
        case "application/vnd.openxmlformats-officedocument.presentationml.presentation":
        case "application/vnd.openxmlformats-officedocument.presentationml.template":
            return "images/icon-m-mime-presentation.png";
        case "application/vnd.oasis.opendocument.text-master":
        case "application/vnd.oasis.opendocument.text":
        case "application/vnd.oasis.opendocument.text-template":
        case "application/msword":
        case "application/rtf":
        case "application/x-mswrite":
        case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
        case "application/vnd.openxmlformats-officedocument.wordprocessingml.template":
        case "application/vnd.ms-works":
            return "images/icon-m-mime-formatted.png";
        case "text/plain":
            return "images/icon-m-mime-plaintext.png";
        case "application/pdf":
            return "images/icon-m-mime-pdf.png";
        default:
            return ""
        }
    }

    //TODO: Convert all component usage to Qt.resolvedUrl once most development is done.
    // Component { id: startPage; StartPage { } }

    //Preload Calligra plugins so we do not need to do that
    //when opening a document so opening becomes more responsive.
    Component.onCompleted: {
        // hack to get the "This device" page the initial page
        var model = documentProviderListModel.sources[0]
        pageStack.push(model.page != "" ? Qt.resolvedUrl(model.page) : fileListPage, {
                  title: model.title,
                  model: model.model,
                  provider: model
        }, PageStackAction.Immediate)
        window.fileListModel = model.model

        if(Qt.application.arguments.length > 1)
            openFile(Qt.application.arguments[1]);
    }

    _defaultPageOrientations: Orientation.Portrait | Orientation.Landscape
    cover: Qt.resolvedUrl("CoverPage.qml")
    Component {
        id: fileListPage
        FileListPage {}
    }
    DocumentProviderListModel {
        id: documentProviderListModel
        TrackerDocumentProvider {}
    }
    DocumentPages {
        id: pages;
    }

    FileInfo {
        id: fileInfo;
    }

    function openFile(file) {
        fileInfo.source = file;

        if(pageStack.currentPage.path === undefined || pageStack.currentPage.path != fileInfo.fullPath)
        {
            switch(Calligra.Global.documentType(fileInfo.fullPath)) {
                case Calligra.DocumentType.TextDocument:
                    pageStack.push(pages.textDocument, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, PageStackAction.Immediate);
                    break;
                case Calligra.DocumentType.Spreadsheet:
                    pageStack.push(pages.spreadsheet, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, PageStackAction.Immediate);
                    break;
                case Calligra.DocumentType.Presentation:
                    pageStack.push(pages.presentation, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, PageStackAction.Immediate);
                    break;
                case Calligra.DocumentType.StaticTextDocument:
                    pageStack.push(pages.pdf, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, PageStackAction.Immediate);
                    break;
                default:
                    console.log("Warning: Unrecognised file type for file " + fileInfo.fullPath);
                    break;
            }
        }
        activate();
    }
}
