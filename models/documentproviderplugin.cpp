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

QUrl DocumentProviderPlugin::page() const
{
    return d->page;
}

void DocumentProviderPlugin::setPage(const QUrl& url)
{
    d->page = url;
    emit pageChanged();
}
