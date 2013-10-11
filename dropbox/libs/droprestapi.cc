/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    License is under GPLv2 <http://www.gnu.org/licenses/gpl-2.0.txt>

*/

#include "droprestapi.h"
#include "json.h"

#include <QDebug>


DropRestAPI::DropRestAPI()
{
    oauth = new OAuth();
}

DropRestAPI::~DropRestAPI(){
    delete oauth;
}

QNetworkRequest DropRestAPI::request_token() {
    QUrl url(REQUEST_TOKEN_URL);
    QNetworkRequest rt;

    rt.setUrl(url);

    oauth->sign("GET", &rt);

    return rt;
}

QNetworkRequest DropRestAPI::request_access_token() {
    QUrl url(REQUEST_ACCESS_TOKEN);
    QNetworkRequest rt;

    rt.setUrl(url);

    oauth->sign("POST", &rt);

    return rt;
}

QNetworkRequest DropRestAPI::root_dir(const QString &folder_name){
    QUrl url(QString("%1%2").arg(FILES_URL).arg(folder_name));
    QNetworkRequest rt;

    rt.setUrl(url);

    oauth->sign("GET", &rt);

    return rt;
}

void DropRestAPI::oauth_request_token_reply_process(QNetworkReply *networkreply){
    QList<QByteArray> oauth_content = networkreply->readAll().split('&');

    oauth->m_secret = oauth_content.at(0).split('=').at(1);
    oauth->m_token = oauth_content.at(1).split('=').at(1);
}

QNetworkRequest DropRestAPI::file_transfer(QString filename, QString dropbox_folder, QString boundaryStr){
   QUrl url;
   QUrlQuery q;
   QNetworkRequest rt;

   url.setUrl(QString("%1%2").arg(FILES_TRANSFER_URL).arg(dropbox_folder));
   q.addQueryItem("file", filename);
   url.setQuery(q);

   rt.setUrl(url);
   rt.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundaryStr);

   oauth->sign("POST", &rt);

   return rt;
}

QNetworkRequest DropRestAPI::file_transfer_download(QString dropbox_filepath){
    QUrl url = QString("%1%2").arg(FILES_TRANSFER_URL).arg(dropbox_filepath);
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__delete(QString dropbox_filepath){
    QUrl url = QString("%1").arg(FILE_DELETE_URL);
    QUrlQuery q;
    q.addQueryItem("root","dropbox");
    q.addQueryItem("path",dropbox_filepath);
    url.setQuery(q);
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__create(QString dropbox_filepath){
    QUrl url = QString("%1").arg(CREATE_FOLDER_URL);
    QUrlQuery q;
    q.addQueryItem("root","dropbox");
    q.addQueryItem("path",dropbox_filepath);
    url.setQuery(q);
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__move(QString path_source, QString path_destination){
    QUrl url = QString("%1").arg(FILE_MOVE_URL);
    QUrlQuery q;
    q.addQueryItem("root", "dropbox");
    q.addQueryItem("from_path", path_source);
    q.addQueryItem("to_path", path_destination);
    url.setQuery(q);

    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__copy(QString path_source, QString path_destination){
    QUrl url = QString("%1").arg(FILE_COPY_URL);
    QUrlQuery q;
    q.addQueryItem("root", "dropbox");
    q.addQueryItem("from_path", path_source);
    q.addQueryItem("to_path", path_destination);
    url.setQuery(q);

    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__shares(QString dropbox_filepath){
    QUrl url = QString("%1").arg(SHARES_URL);
    QUrlQuery q;
    q.addQueryItem("root","dropbox");
    q.addQueryItem("path",dropbox_filepath);
    url.setQuery(q);

    QNetworkRequest rt(url);
    oauth->sign("POST", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::accountinfo() {
    QUrl url = QString("%1").arg(ACCOUNT_INFO_URL);
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}
