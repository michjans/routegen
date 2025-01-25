#include "RGOSMTileProviderManager.h"

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

void RGTileProviderManager::addCustomProvider(const QString& name, const QString& urlTemplate)
{
    if (!providerExists(name))
    {
        customProviders.append(TileProvider(name, urlTemplate));
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
    settings.beginGroup("CustomProviders");
    settings.remove(""); // Clear existing entries
    for (const auto& provider : customProviders)
    {
        settings.setValue(provider.name, provider.urlTemplate);
    }
    settings.endGroup();
}

void RGTileProviderManager::loadCustomProviders()
{
    QSettings settings;
    settings.beginGroup("CustomProviders");
    for (const auto& key : settings.childKeys())
    {
        addCustomProvider(key, settings.value(key).toString());
    }
    settings.endGroup();
}

void RGTileProviderManager::loadDefaultProviders()
{
    defaultProviders.append(TileProvider(QStringLiteral("OpenStreetMap"), QStringLiteral("https://tile.openstreetmap.org/{z}/{x}/{y}.png")));
    defaultProviders.append(TileProvider(QStringLiteral("CycleOSM"), QStringLiteral("https://{s}.tile-cyclosm.openstreetmap.fr/cyclosm/{z}/{x}/{y}.png")));
}
