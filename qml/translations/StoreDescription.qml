import QtQuick 2.0

Item {
    // providing dummy translations for app descriptions shown on Store
    function qsTrIdString() {
        //% "Documents you download to the device appear and can be viewed in Documents app."
        QT_TRID_NOOP("sailfish-office-la-store_app_summary")

        //% "Documents lists every document stored on your device, SD card or other connected storage device. "
        //% "Scroll and zoom opened documents, or jump to a specific part of the document from the attached page. "
        //% "In addition PDF viewer supports text search, annotations and adding comments.\n"
        //% "\n"
        //% "Supported documents and formats include:\n"
        //% " - PDF documents\n"
        //% " - Text documents (.doc, .docx, .odt, .rtf and .txt)\n"
        //% " - Spreadsheets (.xls, .xlsx, .ods and .csv)\n"
        //% " - Presentations (.ppt, .pptx and .odp)\n"
        //% "\n"
        //% "And dozen other less common document formats.\n"
        //% "\n"
        //% "Share your documents easily by just tapping the share button on the toolbar."
        QT_TRID_NOOP("sailfish-office-la-store_app_description")
    }
}
