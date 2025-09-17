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

#ifndef RGWEBMERCATORPROJECTION_H
#define RGWEBMERCATORPROJECTION_H

#include "RGMapProjection.h"

#include "RGGoogleMapBounds.h"
#include "RGOsMapBounds.h"

#include <QGeoCoordinate>
#include <QPoint>
#include <QPointF>

class RGWebMercatorProjection : public RGMapProjection
{
    Q_OBJECT
public:
    /**
     * @brief RGWebMercatorProjection instantiotes the RGWebMercatorProjection from a RGGoogleMapBounds
     * @param mapBounds the RGGoogleMapBounds
     * @param mapWidth width of map
     * @param mapHeight height of map
     * @param parent
     */
    RGWebMercatorProjection(const RGGoogleMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent = nullptr);

    /**
     * @brief RGWebMercatorProjection instantiotes the RGWebMercatorProjection from a RGOsMapBounds
     * @param mapBounds the RGOsMapBounds
     * @param mapWidth width of map
     * @param mapHeight height of map
     * @param parent
     */
    RGWebMercatorProjection(const RGOsMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent = nullptr);

    /**
     * @brief RGWebMercatorProjection instantiotes the RGWebMercatorProjection directly from passed georeference data
     * @param topLeftWorldPixel the topleft pixel in world for the applied zoom
     * @param zoom the used zoomlevel
     * @param mapWidth widht of map
     * @param mapHeight height of map
     * @param parent
     */
    RGWebMercatorProjection(const QPoint topLeftWorldPixel, int zoom, int mapWidth, int mapHeight, QObject* parent = nullptr);
    virtual ~RGWebMercatorProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool fillProjectionTransformation(double& xOrigin, double& yOrigin, double& pixelSize, int& csType) const override;

    QGeoCoordinate pixelToGeo(const QPoint& pixel) const;

    QPoint topLeftWorldPixel() const;
    int zoomLevel() const;

private:
    //Coordinate to world coordinates
    QPointF project(const QGeoCoordinate& geoPoint) const;
    QGeoCoordinate unproject(const QPointF& worldPoint) const;

    //World to pixels
    QPoint worldToPixel(const QPointF& worldPoint) const;
    QPointF pixelToWorld(const QPoint& pixel) const;

    int m_zoom;
    QPoint mTopLeft;
    QPoint mBottomRight;
    int mAntiMeredianPosX;
};

#endif // RGWEBMERCATORPROJECTION_H
