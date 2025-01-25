/*
    Copyright (C) 2008-2024  Michiel Jansen

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

#ifndef RGOSMBACKEND_H
#define RGOSMBACKEND_H

#include "RGOSMTileProviderManager.h"

#include <QImage>
#include <QObject>
#include <QRandomGenerator>
#include <QtNetwork/QNetworkAccessManager>

class RGOsmBackend : public QObject
{
    Q_OBJECT
public:
    RGOsmBackend(QObject* parent = nullptr);

    void setTileProvider(const RGTileProviderManager::TileProvider& provider);
    /**
     * @brief requestTile Asynchronous method to request a tile in the background and get signalled when the requested
     *        tile is available @see tileAvailable
     */
    void requestTile(int x, int y, int zoom);
    QImage getTile(int x, int y, int zoom);
    void stitchTiles(double lat, double lon, int zoom, int width, int height, const QString& outputFile);

signals:
    void tileAvailable(const QImage& tile, int x, int y);

private:
    void downloadTileAndSaveToCache(int x, int y, int zoom);
    QString getTileUrl(int x, int y, int zoom);
    void addAttribution(QImage& image, const QString& attributionText);

    QString getCachePath(int zoom, int x, int y);
    bool isTileCached(int zoom, int x, int y);
    QImage loadCachedTile(int zoom, int x, int y);
    void saveTileToCache(const QImage& tile, int zoom, int x, int y);

    QNetworkAccessManager m_manager;
    QRandomGenerator m_randommizer;

    const QString mCachePath;
    RGTileProviderManager::TileProvider mTileProvider;
    QStringList mSubDomains = {"a", "b", "c"};
};

#endif // RGOSMBACKEND_H
