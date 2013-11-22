import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Office 1.0
import org.kde.calligra 1.0 as Calligra
import Sailfish.Office.Files 1.0

ApplicationWindow
{
    id: window;
    cover: "";

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

        if(Qt.application.arguments.length > 1)
            openFile(Qt.application.arguments[1], PageStackAction.Immediate);
    }
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

    function openFile(file, operationType) {
        fileInfo.source = file;

        if(pageStack.currentPage.path === undefined || pageStack.currentPage.path != fileInfo.fullPath)
        {
            switch(Calligra.Global.documentType(fileInfo.fullPath)) {
                case Calligra.DocumentType.TextDocument:
                    pageStack.push(pages.textDocument, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, operationType);
                    break;
                case Calligra.DocumentType.Spreadsheet:
                    pageStack.push(pages.spreadsheet, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, operationType);
                    break;
                case Calligra.DocumentType.Presentation:
                    pageStack.push(pages.presentation, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, operationType);
                    break;
                case Calligra.DocumentType.StaticTextDocument:
                    pageStack.push(pages.pdf, { title: fileInfo.fileName, path: fileInfo.fullPath, mimeType: fileInfo.mimeType }, operationType);
                    break;
                default:
                    console.log("Warning: Unrecognised file type for file " + fileInfo.fullPath);
                    break;
            }
        }
        activate();
    }
}
