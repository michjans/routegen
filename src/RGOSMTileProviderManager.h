/*
    Copyright (C) 2008-2025  Michiel Jansen

This file is part of Route Generator.

Route Generator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

    // Add a custom provider, returns true when provider was added
    bool addCustomProvider(const TileProvider& tileProvider);

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
