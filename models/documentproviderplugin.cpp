#include "documentproviderplugin.h"

class DocumentProviderPlugin::Private {
public:
    Private() {}
    QUrl page;
};

DocumentProviderPlugin::DocumentProviderPlugin(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

DocumentProviderPlugin::~DocumentProviderPlugin()
{
    delete d;
}

QString DocumentProviderPlugin::setupPageUrl() const
{
    return QString();
}

bool DocumentProviderPlugin::needsSetup() const
{
    return false;
}

QUrl DocumentProviderPlugin::page() const
{
    return d->page;
}

void DocumentProviderPlugin::setPage(const QUrl& url)
{
    d->page = url;
    emit pageChanged();
}

void DocumentProviderPlugin::deleteFile(const QUrl& file)
{
    Q_UNUSED(file);
    qWarning("Provider does not implement file deletion.");
}
