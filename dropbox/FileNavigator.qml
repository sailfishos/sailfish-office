import QtQuick 1.1
import Sailfish.Silica 1.0
import Sailfish.Office 1.0
import Sailfish.Office.Files 1.0

Page {
    id: page;
    property QtObject model: null
    property string title: "";
    property string errorText: "";
    property bool have_checked: false;
    property string currentDir: "/";

    property string fileMimetype;
    property string fileName;

    InfoBanner { id: i_infobanner; }
    Component {
        id: itemDelegate;
        BackgroundItem {
            width: ListView.view.width;
            height: theme.itemSizeLarge;

            Image {
                id: icon;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    topMargin: theme.paddingSmall;
                }

                source: model.is_dir ? "" : "image://theme/icon-l-document"
            }

            Label {
                anchors {
                    left: icon.right;
                    leftMargin: theme.paddingSmall;
                    right: parent.right;
                    top: parent.top;
                    topMargin: theme.paddingSmall;
                }
                text:{
                    var nameof = model.path.split("/")
                    var nameof1 = nameof[nameof.length - 1]

                    var maxlength = 30
                    if (page.width > page.height)
                        maxlength = 70

                    if (nameof1.length >= maxlength)
                        return nameof1.substring(0,maxlength-10) + " ... " + nameof1.substring(nameof1.length-10,nameof1.length)
                    else
                        return nameof1
                }
                elide: Text.ElideRight;
                font.pixelSize: theme.fontSizeLarge;
                font.bold: model.is_dir
            }
            Label {
                anchors {
                    left: icon.right;
                    leftMargin: theme.paddingSmall;
                    bottom: parent.bottom;
                    bottomMargin: theme.paddingSmall;
                }
                visible: !model.is_dir;
                text: model.size;
                font.pixelSize: theme.fontSizeSmall;
                color: theme.secondaryColor;
            }
            Label {
                anchors {
                    right: parent.right;
                    rightMargin: theme.paddingLarge;
                    bottom: parent.bottom;
                    bottomMargin: theme.paddingSmall;
                }
                text: Qt.formatDate(model.modified.split("+")[0]);
                font.pixelSize: theme.fontSizeSmall;
                color: theme.secondaryColor;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if(model.is_dir)
                    {
                        controllerMIT.setFolderRoot(model.path)
                        controllerMIT.getlistoffolder()
                    }
                    else
                    {
                        controllerMIT.setCheck(index, true);
                        controllerMIT.downloadSelectedFiles();
                        var nameof = model.path.split("/");
                        page.fileName = nameof[nameof.length - 1];
                        page.fileMimetype = model.mime_type;
                        pageStack.push(downloadStatus);
                        controllerMIT.start_transfer_process();
                    }
                }
            }
        }
    }

    Page {
        id: downloadStatus;
        SilicaListView {
            id: filestransferListView
            anchors.fill: parent
            anchors.topMargin: 71
            model: filesTransferModel
            delegate: Item {
                width: downloadStatus.width
                height: 60

                Rectangle {
                    id:rec_main
                    anchors.fill: parent
                    anchors.leftMargin: -1
                    anchors.rightMargin: -1
                    anchors.bottomMargin: 0
                    color: "transparent"

                    Image {
                        id: i_right
                        x:5
                        anchors.verticalCenter: parent.verticalCenter
                        //source: "imgs/file.png"
                    }

                    Label {
                        id: l_name
                        x: i_right.width + 10
                        y:5
                        text: {
                            var filename1 = filename.split("/")
                            var filename2 = filename1[filename1.length-1]

                            var maxlength = 30
                            if (downloadStatus.width > downloadStatus.height)
                                maxlength = 70

                            if (filename2.length >= maxlength)
                                return filename2.substring(0,maxlength-10) + " ... " + filename2.substring(filename2.length-10,filename2.length)
                            else
                                return filename2
                        }
                        font.pixelSize: 20
                        //color:"black"
                    }

                    Label {
                        id: l_status
                        y: l_name.y + l_name.height
                        x: i_right.width + 10
                        visible: is_finished || in_queue
                        text: {
                            if (is_finished)
                                return (completed ? "Completed":"Failed ("+(is_cancelled?"it was cancelled by user":"network error")+")")+" "+(completed?"("+date+")":"")
                            else {
                                var dropbox_path1 = dropbox_path
                                var maxlength = 30
                                if (downloadStatus.width > downloadStatus.height)
                                    maxlength = 70
                                if (dropbox_path && dropbox_path.length >= maxlength)
                                    dropbox_path1 = dropbox_path.substring(0,maxlength-10) + " ... " + dropbox_path.substring(dropbox_path.length-10,dropbox_path.length)

                                //return "Pending ("+(dropbox_path1?dropbox_path1:"/")+")"
                                return model.size+" | Pending..." //size of download or upload file
                            }

                        }
                        font.pixelSize: 18
                        color: completed ? "#8f5555" : theme.secondaryColor
                    }

                    ProgressBar {
                        id: pb_updown
                        y: l_name.y + l_name.height
                        x: i_right.width + 10
                        visible: !is_finished && !in_queue
                        width: parent.width - 120
                        minimumValue: 1
                        maximumValue: 100
                        indeterminate : progressing ? false : true
                        value: progressing
                    }

                    Label {
                        id: lb_updown_total
                        y: pb_updown.y+pb_updown.height
                        x: i_right.width + 10
                        visible: !is_finished && !in_queue
                        font.pixelSize: 18
                        text:""
                    }

                    Connections {
                        target: controllerMIT
                        onProgressBarChanged : {
                            pb_updown.value = percent
                            if (sent_received || speed){
                                lb_updown_total.text = (is_download?"Received: ":"Sent: ")+__sent_received_calculate(sent_received) + " | Speed: " +__speed_calculate(speed)
                            }else
                                lb_updown_total.text = ""
                        }
                    }

                    Image {
                        id: i_fstatus
                        x:parent.width - 40
                        anchors.verticalCenter: parent.verticalCenter
                        source: is_download ? "image://theme/icon-s-cloud-download" : "image://theme/icon-s-cloud-upload"
                    }
                }
            } //end filestransferDeligate
            onCountChanged: {
                filestransferListView.visible = (filestransferListView.model.count !== 0)
                //no_transfers.visible = !filestransferListView.model.count
            }
        } //end ListView
    }

    SilicaListView {
        id: folderListView
        visible:false
        anchors.fill: parent;
        model: folderListModel
        delegate: itemDelegate
        cacheBuffer: 1000
        section.property: "section"
        clip: true
        header: Item {
            id: pageHeader;
            height: pageHead.height + backToRootButton.height;
            width: page.width;
            PageHeader {
                id: pageHead;
                title: page.title;
                anchors.top: parent.top;
            }
            BackgroundItem {
                id: backToRootButton;
                anchors {
                    top: pageHead.bottom;
                    margins: theme.paddingSmall
                }
                width: page.width;
                opacity: currentDir !== "/" ? 1 : 0;
                Behavior on opacity{ PropertyAnimation{ duration: 150; } }
                Image {
                    id: icon;
                    anchors {
                        left: parent.left;
                        top: parent.top;
                        topMargin: theme.paddingSmall;
                    }

                    source: "";
                }

                Label {
                    anchors {
                        left: icon.right;
                        leftMargin: theme.paddingSmall;
                        right: parent.right;
                        top: parent.top;
                        topMargin: theme.paddingSmall;
                    }
                    text: ".. (up one level)"
                    elide: Text.ElideRight;
                    font.pixelSize: theme.fontSizeLarge;
                    font.bold: true
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        controllerMIT.backtoRootDir()
                        controllerMIT.getlistoffolder()
                    }
                }
            }
        }
    }

    Item {
        id:this_folder_is_empty
        visible: false
        anchors.fill: parent
        Label {
            anchors.centerIn: parent
            text:"This Folder is Empty"
            color: "grey"
        }
    }

    Item {
        id:r_networkerror
        visible: false
        anchors.centerIn: parent
        width: parent.width - theme.paddingLarge
        height: r_label_x.height + r_button_x.height;
        Label {
            id: r_label_x
            anchors {
                bottom: r_button_x.top;
                left: parent.left;
                right: parent.right;
                margins: theme.paddingLarge;
            }
            height: paintedHeight;
            text: "There was a problem loading your Dropbox. It could be lost connection or a slow network. Check your connection or try again later. The reported error was: " + errorText;
            wrapMode: Text.Wrap;
        }
        Button {
            id:r_button_x
            text:"Retry"
            anchors {
                horizontalCenter: parent.horizontalCenter;
                bottom: parent.bottom;
            }
            onClicked: {
                r_networkerror.visible = false
                b_indicator.visible = true
                b_indicator_wrap.visible=true
                controllerMIT.refresh_current_folder()
            }
        }
    }

    ProgressCircle {
        id: b_indicator
        anchors.centerIn: parent
        Timer {
            interval: 10
            repeat: true
            onTriggered: b_indicator.value = (b_indicator.value + 0.005) % 1.0
            running: b_indicator.visible;
        }
    }

    MouseArea {
        id: b_indicator_wrap
        anchors.fill: parent
        visible: true
    }

    Connections {
        target: controllerMIT

        onFolderfinished : {
            r_networkerror.visible = false
            folderListView.visible = true
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            tb_back.enabled = !controllerMIT.isRootDir()
            //f_fastscroll.test()
            //top_banner.havemenu = true

            currentDir = controllerMIT.getcurrentdir()
            changeCurrentDir()

            this_folder_is_empty.visible = folderListModel.count ? false : true

            //folderListView.visible = folderListModel.count ? true : false
            folderListView.visible = folderListModel.count ? true : true
        }

        onNetwork_error : { //error
            //top_banner.havemenu = false
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            folderListView.visible = false
            errorText = error;
            r_networkerror.visible = true
        }

        onNotification : {
            i_infobanner.text = notification
            i_infobanner.visible = true
            console.log("notification:"+ notification)
        }

        onDelete_selected_items_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            i_infobanner.text = result
            i_infobanner.visible = true
            console.log("onDelete_selected_items_finished:" + result);
        }

        onEnable_download_and_delete_button: {
            console.log("onEnable_download_and_delete_button")
            //top_banner.deleteVisible = have_checked
            //top_banner.addDownloadVisible = have_checked
            //top_banner.moveVisible = have_checked

            page.have_checked = have_checked
        }

        onStopTransfer: {
            if (!controllerMIT.is_push_notification()){
                i_infobanner.text = "Files transfer completed"
                i_infobanner.visible = true
            }
            refreshDir()
            var filePath = controllerMIT.dropboxFolder() + "/" + page.fileName;
            var docClass = page.model.mimeTypeToDocumentClass(page.fileMimetype);
            switch(docClass) {
                case DocumentListModel.TextDocument:
                    pageStack.push(textDocumentPage, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
                case DocumentListModel.SpreadSheetDocument:
                    pageStack.push(spreadsheetPage, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
                case DocumentListModel.PresentationDocument:
                    pageStack.push(presentationPage, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
                case DocumentListModel.PDFDocument:
                    pageStack.push(pdfPage, { title: page.fileName, path: filePath, mimeType: page.fileMimetype });
                default:
                    console.log("Unknown file format " + docClass + " for file " + filePath + " with stated mimetype " + page.fileMimetype);
            }
        }

        onCreate_folder_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            i_infobanner.text = result
            i_infobanner.visible = true
            console.log("onCreate_folder_finished:" + result)
        }

        onRename_folder_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            i_infobanner.text = result
            i_infobanner.visible = true
        }

        onMove_files_folders_finished: {
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            m_is_move = false
            m_is_copy = false
            toolicon_show(true);
            i_infobanner.text = result
            i_infobanner.visible = true
        }

        onShares_finished:{ //result
            b_indicator.visible = false; b_indicator_wrap.visible=false;
            if (!result){
                i_infobanner.text = "Could not create share link, try again later."
                i_infobanner.visible = true
            }
        }

        onShares_metadata:{ //url, expire
            sharemetadatadlg.url = url
            pageStack.push(sharemetadatadlg)
        }

        onAccountinfo_finished: { //result
            b_indicator.visible = false; b_indicator_wrap.visible=false;
        }

        onAccountinfo_metadata: { //result
            var val=[]
            for(var i=0; i<6;i++){
                if (i==0)
                    val.push(result['display_name'])
                else if(i==1)
                    val.push(result['email'])
                else if(i==2)
                    val.push(result['uid'])
                else if(i==3)
                    val.push(__convertToMB(result['quota_info']['shared']))
                else if(i==4)
                    val.push(__convertToMB(result['quota_info']['quota']))
                else
                    val.push(__convertToMB(result['quota_info']['normal']))
            }
            accountinfodlg.m_data = val
            pageStack.push(accountinfodlg)
        }
    }

    Component.onCompleted: {
        controllerMIT.need_authenticate()
        controllerMIT.getlistoffolder()
    }

    IconButton {
        id:tb_back
        anchors {
            bottom: page.bottom;
            right: page.right;
        }
        enabled: !controllerMIT.isRootDir()
        onClicked: {
            controllerMIT.backtoRootDir()
            controllerMIT.getlistoffolder()
        }
    }

    function changeCurrentDir(){
         var maxlength = 30
         if (page.width > page.height)
               maxlength = 70

         if (!currentDir.length) currentDir = "/"
            if (currentDir.length >= maxlength)
                 currentDir = currentDir.substring(0,maxlength-10) + " ... " + currentDir.substring(currentDir.length-10,currentDir.length)
    }
    function refreshDir(){
        b_indicator.visible = true; b_indicator_wrap.visible=true;
        controllerMIT.refresh_current_folder()
    }

    function toFixed(value, precision) {
        var power = Math.pow(10, precision || 0);
        return String(Math.round(value * power) / power);
    }
    function __sent_received_calculate(bytes){
        if (bytes < 1024)
            return toFixed(bytes,2)+ " bytes"
        else
        if (bytes < 1024*1024)
            return toFixed((bytes/1024),2)+ " KB"
        else
            return toFixed(((bytes/1024)/1024),2) + " MB"
    }

    function __speed_calculate(bytes){
        if (bytes < 1024)
            return toFixed(bytes,2) + " bps"
        else
            if(bytes < 1024*1024)
                return toFixed((bytes/1024),2) + " Kbps"
        else
                return toFixed(((bytes/1024)/1024),2) + " Mbps"
    }

    Component {
        id: textDocumentPage;
        TextDocumentPage { }
    }

    Component {
        id: spreadsheetPage;
        SpreadsheetPage { }
    }

    Component {
        id: presentationPage;
        PresentationPage { }
    }

    Component {
        id: pdfPage;
        PDFDocumentPage { }
    }
}