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

#include <QImage>
#include <QRandomGenerator>
#include <QtNetwork/QNetworkAccessManager>

class RGOsmBackend
{
public:
    RGOsmBackend();

    QImage getTile(int x, int y, int zoom);
    void stitchTiles(double lat, double lon, int zoom, int width, int height, const QString& outputFile);

private:
    QImage downloadTile(int x, int y, int zoom);
    QString getTileUrl(int x, int y, int zoom, const QStringList& subdomains);
    void addAttribution(QImage& image, const QString& attributionText);

    QNetworkAccessManager m_manager;
    QRandomGenerator m_randommizer;
};

#endif // RGOSMBACKEND_H
