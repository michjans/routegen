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
        QString attribution;

        TileProvider()
        {
        }

        TileProvider(const QString& name, const QString& urlTemplate, const QString& attribution)
            : name(name),
              urlTemplate(urlTemplate),
              attribution(attribution)
        {
        }
    };

    RGTileProviderManager();

    // Get all tile providers (default + custom)
    QVector<TileProvider> getAllProviders() const;

    // Get only the custom providers
    QVector<TileProvider> getCustomProviders() const;

    // Add a custom provider
    void addCustomProvider(const TileProvider& tileProvider);

    // Returns TileProvider by name (return nullptr if not found)
    const TileProvider* getCustomProviderByName(const QString& name);

    // Replace exiting custom provider (name is key)
    void replaceCustomProvider(const TileProvider& tileProvider);

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
