#include "RGOSMTileProviderManager.h"

#include <QHash>
#include <QMap>
#include <QSettings>

RGTileProviderManager::RGTileProviderManager()
{
    loadDefaultProviders();
    loadCustomProviders();
}

QVector<RGTileProviderManager::TileProvider> RGTileProviderManager::getAllProviders() const
{
    QVector<RGTileProviderManager::TileProvider> allProviders = defaultProviders;
    allProviders.append(customProviders);
    return allProviders;
}

QVector<RGTileProviderManager::TileProvider> RGTileProviderManager::getCustomProviders() const
{
    return customProviders;
}

void RGTileProviderManager::addCustomProvider(const TileProvider& tileProvider)
{
    if (!providerExists(tileProvider.name))
    {
        customProviders.append(tileProvider);
    }
}

bool RGTileProviderManager::removeCustomProvider(const QString& name)
{
    for (int i = 0; i < customProviders.size(); ++i)
    {
        if (customProviders[i].name == name)
        {
            customProviders.removeAt(i);
            return true;
        }
    }
    return false;
}

bool RGTileProviderManager::providerExists(const QString& name) const
{
    for (const auto& provider : defaultProviders)
    {
        if (provider.name == name)
            return true;
    }
    for (const auto& provider : customProviders)
    {
        if (provider.name == name)
            return true;
    }
    return false;
}

void RGTileProviderManager::saveCustomProviders() const
{
    QSettings settings;
    QMap<QString, QVariant> osmTileProviderMap = settings.value("customOsmTileProviders", QMap<QString, QVariant>()).toMap();
    for (const auto& provider : customProviders)
    {
        QHash<QString, QVariant> cpMap;
        cpMap[QStringLiteral("urlTemplate")] = provider.urlTemplate;
        cpMap[QStringLiteral("attribution")] = provider.attribution;

        osmTileProviderMap[provider.name] = QVariant(cpMap);
    }
    settings.setValue("customOsmTileProviders", osmTileProviderMap);
}

void RGTileProviderManager::loadCustomProviders()
{
    QSettings settings;
    QMap<QString, QVariant> osmTileProviderMap = settings.value("customOsmTileProviders", QMap<QString, QVariant>()).toMap();
    QMapIterator<QString, QVariant> it(osmTileProviderMap);
    while (it.hasNext())
    {
        it.next();
        QHash<QString, QVariant> cpMap = it.value().toHash();
        TileProvider tileProvider;
        tileProvider.name = it.key();
        tileProvider.urlTemplate = cpMap[QStringLiteral("urlTemplate")].toString();
        tileProvider.attribution = cpMap[QStringLiteral("attribution")].toString();
        addCustomProvider(tileProvider);
    }
}

void RGTileProviderManager::loadDefaultProviders()
{
    defaultProviders.append(TileProvider(QStringLiteral("OpenStreetMap"), QStringLiteral("https://tile.openstreetmap.org/{z}/{x}/{y}.png"),
                                         QStringLiteral("© OpenStreetMap contributors")));
    defaultProviders.append(TileProvider(QStringLiteral("CycleOSM"), QStringLiteral("https://{s}.tile-cyclosm.openstreetmap.fr/cyclosm/{z}/{x}/{y}.png"),
                                         QStringLiteral("© OpenStreetMap contributors | Tiles courtesy of CycleOSM")));
}
