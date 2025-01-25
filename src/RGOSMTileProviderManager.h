#ifndef RGOSMTILEPROVIDERMANAGER_H
#define RGOSMTILEPROVIDERMANAGER_H

#include <QString>
#include <QVector>

class RGTileProviderManager
{
public:
    struct TileProvider
    {
        QString name;
        QString urlTemplate;

        TileProvider()
        {
        }

        TileProvider(const QString& name, const QString& urlTemplate)
            : name(name),
              urlTemplate(urlTemplate)
        {
        }
    };

    RGTileProviderManager();

    // Get all tile providers (default + custom)
    QVector<TileProvider> getAllProviders() const;

    // Add a custom provider
    void addCustomProvider(const QString& name, const QString& urlTemplate);

    // Remove a custom provider by name
    bool removeCustomProvider(const QString& name);

    // Check if a provider exists
    bool providerExists(const QString& name) const;

    void saveCustomProviders() const;

    void loadCustomProviders();

private:
    QVector<TileProvider> defaultProviders;
    QVector<TileProvider> customProviders;

    void loadDefaultProviders(); // Initialize default providers
};

#endif // RGOSMTILEPROVIDERMANAGER_H
