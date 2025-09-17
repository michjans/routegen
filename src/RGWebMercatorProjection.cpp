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

#include "RGWebMercatorProjection.h"

#include <QDebug>
#include <QtMath>

namespace
{
const int TILE_SIZE = 256;
const double R = 6378137.0;

QPointF lonLatToMeters(const QGeoCoordinate& coord)
{
    double mx = R * coord.longitude() * M_PI / 180.0;
    double my = R * std::log(std::tan(M_PI / 4.0 + coord.latitude() * M_PI / 360.0));
    return QPointF(mx, my);
}
}

RGWebMercatorProjection::RGWebMercatorProjection(const RGGoogleMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent)
    : RGMapProjection(parent),
      m_zoom(mapBounds.getZoom())
{
    //Calculate topleft/bottomright of the current map in pixel coordinates
    mTopLeft = worldToPixel(project(QGeoCoordinate(mapBounds.getNE().latitude(), mapBounds.getSW().longitude())));
    mBottomRight = worldToPixel(project(QGeoCoordinate(mapBounds.getSW().latitude(), mapBounds.getNE().longitude())));
    mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();

    qDebug() << "zoom:" << mapBounds.getZoom();
    qDebug() << "pixWidth:" << mBottomRight.x() - mTopLeft.x();
    qDebug() << "pixHeigth:" << mBottomRight.y() - mTopLeft.y();
    qDebug() << "mapBounds:" << mapBounds.toQVariant();
}

RGWebMercatorProjection::RGWebMercatorProjection(const RGOsMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent)
    : RGMapProjection(parent),
      m_zoom(mapBounds.getZoom())
{
    QPointF worldCenter = project(mapBounds.getCenterCoord());
    QPoint centerPixel = worldToPixel(worldCenter);

    mTopLeft = centerPixel - QPoint(mapWidth / 2, mapHeight / 2);
    mBottomRight = centerPixel + QPoint(mapWidth / 2, mapHeight / 2);
    mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();

    qDebug() << "zoom:" << m_zoom;
    qDebug() << "pixWidth:" << mBottomRight.x() - mTopLeft.x();
    qDebug() << "pixHeigth:" << mBottomRight.y() - mTopLeft.y();
}

RGWebMercatorProjection::RGWebMercatorProjection(const QPoint topLeftWorldPixel, int zoom, int mapWidth, int mapHeight, QObject* parent)
    : RGMapProjection(parent),
      m_zoom(zoom),
      mTopLeft(topLeftWorldPixel),
      mBottomRight(mTopLeft + QPoint(mapWidth, mapHeight))
{
    mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();
}

RGWebMercatorProjection::~RGWebMercatorProjection()
{
}

bool RGWebMercatorProjection::isValid() const
{
    return !mTopLeft.isNull() && !mBottomRight.isNull();
}

QPoint RGWebMercatorProjection::convert(const QGeoCoordinate& geoPoint) const
{
    QPoint point = worldToPixel(project(geoPoint));
    //TODO: This check will not work anymore since we refactored this class to support OSM imports and saving
    //      geo projections by just saving the mTopLeft and zoomlevel!
    if (mTopLeft.x() > mBottomRight.x() && point.x() < mTopLeft.x())
    {
        //When the route passes around the 180.0/-180.0 meridian (antimeridian), the coordinates will wrap around and we need to correct for this
        //by calculating the x pixel position of the antimeridan boundary and add that to all coordinates < the top left x value
        point.setX(mAntiMeredianPosX + point.x());
    }

    return point - mTopLeft;
}

bool RGWebMercatorProjection::fillProjectionTransformation(double& xOrigin, double& yOrigin, double& pixelSize, int& csType) const
{
    if (!isValid())
    {
        return false;
    }

    // meters per pixel
    pixelSize = 2.0 * M_PI * R / (TILE_SIZE * (1 << m_zoom));

    // compute geo position of top-left pixel (EPSG:3857 meters)
    QGeoCoordinate topLeftCoord = pixelToGeo(QPoint(0, 0));
    QPointF topLeftMeters = lonLatToMeters(topLeftCoord);
    qDebug() << "RGWebMercatorProjection: topLeftCoord = " << topLeftCoord;
    qDebug() << "RGWebMercatorProjection: topLeftMeters = " << topLeftMeters;
    qDebug() << "RGWebMercatorProjection: pixelSize = " << pixelSize;

    xOrigin = topLeftMeters.x();
    yOrigin = topLeftMeters.y();
    csType = 3857; // EPSG:3857

    return true;
}

QGeoCoordinate RGWebMercatorProjection::pixelToGeo(const QPoint& pixel) const
{
    // Re-add the top-left offset
    QPoint absPixel = pixel + mTopLeft;

    // If the map crosses the antimeridian, normalize the x coordinate
    //TODO: This check will not work anymore since we refactored this class to support OSM imports and saving
    //      geo projections by just saving the mTopLeft and zoomlevel!
    if (mTopLeft.x() > mBottomRight.x() && absPixel.x() > mAntiMeredianPosX)
    {
        absPixel.setX(absPixel.x() - mAntiMeredianPosX);
    }

    QPointF world = pixelToWorld(absPixel);
    return unproject(world);
}

QPoint RGWebMercatorProjection::topLeftWorldPixel() const
{
    return mTopLeft;
}

int RGWebMercatorProjection::zoomLevel() const
{
    return m_zoom;
}

QPointF RGWebMercatorProjection::project(const QGeoCoordinate& geoPoint) const
{
    qreal siny = qSin(geoPoint.latitude() * M_PI / 180.0);

    // Algorithm copied from Google maps API
    // Truncating to 0.9999 effectively limits latitude to 89.189. This is
    // about a third of a tile past the edge of the world tile.
    siny = std::min(std::max(siny, -0.9999), 0.9999);

    return QPointF(TILE_SIZE * (0.5 + geoPoint.longitude() / 360), TILE_SIZE * (0.5 - std::log((1 + siny) / (1 - siny)) / (4 * M_PI)));
}

QGeoCoordinate RGWebMercatorProjection::unproject(const QPointF& worldPoint) const
{
    const double xnorm = worldPoint.x() / TILE_SIZE;
    const double ynorm = worldPoint.y() / TILE_SIZE;

    const double lon = (xnorm - 0.5) * 360.0;

    const double n = M_PI - 2.0 * M_PI * ynorm;
    const double lat = 180.0 / M_PI * std::atan(std::sinh(n));
    return QGeoCoordinate(lat, lon);
}

QPoint RGWebMercatorProjection::worldToPixel(const QPointF& worldPoint) const
{
    int scale = 1 << m_zoom;

    return QPoint(qFloor(worldPoint.x() * scale), qFloor(worldPoint.y() * scale));
}

QPointF RGWebMercatorProjection::pixelToWorld(const QPoint& pixel) const
{
    int scale = 1 << m_zoom;
    return QPointF(static_cast<qreal>(pixel.x()) / scale, static_cast<qreal>(pixel.y()) / scale);
}
