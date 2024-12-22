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

#include "RGOsmBackend.h"
#include "RGOSMapProjection.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QStandardPaths>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <cmath>

RGOsmBackend::RGOsmBackend()
    : mCachePath(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/osmcache/%1/%2/%3.png")
{
}

QImage RGOsmBackend::getTile(int x, int y, int zoom)
{
    if (isTileCached(zoom, x, y))
    {
        return loadCachedTile(zoom, x, y);
    }
    else
    {
        QImage tile = downloadTile(x, y, zoom);
        if (!tile.isNull())
        {
            saveTileToCache(tile, zoom, x, y);
        }
        return tile;
    }
}

void RGOsmBackend::stitchTiles(double lat, double lon, int zoom, int width, int height, const QString& outputFile)
{
    // Calculate the number of tiles needed to cover the desired area
    QPointF centerTile = RGOSMapProjection::latLonToTile(QGeoCoordinate(lat, lon), zoom);
    int tileXCenter = std::floor(centerTile.x());
    int tileYCenter = std::floor(centerTile.y());

    int tilesX = std::ceil((double)width / RGOSMapProjection::TILE_SIZE);
    int tilesY = std::ceil((double)height / RGOSMapProjection::TILE_SIZE);

    int tileXMin = tileXCenter - tilesX / 2;
    int tileYMin = tileYCenter - tilesY / 2;

    QImage stitchedImage(width, height, QImage::Format_ARGB32);
    stitchedImage.fill(Qt::white);

    QPainter painter(&stitchedImage);

    // Download and draw each tile
    for (int x = 0; x <= tilesX; ++x)
    {
        for (int y = 0; y <= tilesY; ++y)
        {
            int tileX = tileXMin + x;
            int tileY = tileYMin + y;

            QImage tile = getTile(tileX, tileY, zoom);
            if (!tile.isNull())
            {
                int posX = x * RGOSMapProjection::TILE_SIZE - ((centerTile.x() - tileXCenter) * RGOSMapProjection::TILE_SIZE);
                int posY = y * RGOSMapProjection::TILE_SIZE - ((centerTile.y() - tileYCenter) * RGOSMapProjection::TILE_SIZE);
                painter.drawImage(posX, posY, tile);
            }
        }
    }

    painter.end();

    // Add attribution
    addAttribution(stitchedImage, QStringLiteral(u"© OpenStreetMap contributors"));

    // Save the result
    if (stitchedImage.save(outputFile))
    {
        qDebug() << "Map saved to " << outputFile;
    }
    else
    {
        qWarning() << "Failed to save map to " << outputFile;
    }
}

// Downloads a tile from OpenStreetMap
QImage RGOsmBackend::downloadTile(int x, int y, int zoom)
{
    QStringList subdomains = {"a", "b", "c"};
    QString url = getTileUrl(x, y, zoom, subdomains);
    qDebug() << "Downloading tile " << url;
    QNetworkRequest request((QUrl(url)));
    request.setRawHeader("User-Agent", "RouteGenerator/1.13.0 (info@routegenerator.net)");
    QNetworkReply* reply = m_manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        QImage tile;
        tile.loadFromData(data);
        reply->deleteLater();
        return tile;
    }
    else
    {
        qWarning() << "Failed to download tile: " << reply->errorString();
        reply->deleteLater();
        return QImage();
    }
}

QString RGOsmBackend::getTileUrl(int x, int y, int zoom, const QStringList& subdomains)
{
    //QString urlTemplate = "https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png";
    QString urlTemplate = "https://{s}.tile-cyclosm.openstreetmap.fr/cyclosm/{z}/{x}/{y}.png";

    const QString& subdomain = subdomains[m_randommizer.bounded(0, subdomains.size())]; // Random subdomain
    return urlTemplate.replace("{s}", subdomain).replace("{z}", QString::number(zoom)).replace("{x}", QString::number(x)).replace("{y}", QString::number(y));
}

void RGOsmBackend::addAttribution(QImage& image, const QString& attributionText)
{
    QPainter painter(&image);

    // Set font and color
    QFont font("Arial", 12); // Adjust font size as needed
    painter.setFont(font);
    painter.setPen(QColor(0, 0, 0, 200)); // Semi-transparent black

    // Calculate position
    QFontMetrics metrics(font);
    int textWidth = metrics.horizontalAdvance(attributionText);
    int textHeight = metrics.height();

    int padding = 10;                            // Space from the edges
    int x = image.width() - textWidth - padding; // Right-align text
    int y = image.height() - padding;            // Bottom of the image

    // Add background rectangle for better readability
    QRect backgroundRect(x - padding / 2, y - textHeight - padding / 2, textWidth + padding, textHeight + padding);
    painter.fillRect(backgroundRect, QColor(255, 255, 255, 150)); // Semi-transparent white background

    // Draw text
    painter.drawText(x, y, attributionText);

    painter.end();
}

QString RGOsmBackend::getCachePath(int zoom, int x, int y)
{
    return mCachePath.arg(zoom).arg(x).arg(y);
}

bool RGOsmBackend::isTileCached(int zoom, int x, int y)
{
    QString path = getCachePath(zoom, x, y);
    QFile file(path);
    return file.exists();
}

QImage RGOsmBackend::loadCachedTile(int zoom, int x, int y)
{
    QString path = getCachePath(zoom, x, y);
    QImage tile;
    qDebug() << "loading tile from cache: " << path;
    if (tile.load(path))
    {
        return tile;
    }
    else
    {
        return QImage();
    }
}

void RGOsmBackend::saveTileToCache(const QImage& tile, int zoom, int x, int y)
{
    QString path = getCachePath(zoom, x, y);

    // Ensure the directory exists
    QDir dir;
    dir.mkpath(QFileInfo(path).path());

    // Save the tile
    if (!tile.save(path))
    {
        qWarning() << "Failed to save tile to " << path;
    }
}

/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

           // Example: Center on Eiffel Tower with zoom 15, output size 1920x1080
    double lat = 48.8588443; // Latitude of the center
    double lon = 2.2943506;  // Longitude of the center
    int zoom = 15;           // Zoom level
    int width = 1920;        // Output image width
    int height = 1080;       // Output image height
    QString outputFile = "map.png";

    stitchTiles(lat, lon, zoom, width, height, outputFile);

    return 0;
}
*/
