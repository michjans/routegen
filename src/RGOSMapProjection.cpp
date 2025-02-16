#include "RGOSMapProjection.h"
#include "RGSettings.h"

#include <QDebug>
#include <QtMath>

RGOSMapProjection::RGOSMapProjection(const RGOsMapBounds& osmBoumds, int mapWidth, int mapHeight, QObject* parent)
    : RGMapProjection(parent),
      m_osmBoumds(osmBoumds),
      m_mapWidth(mapWidth),
      m_mapHeight(mapHeight)
{
}

RGOSMapProjection::~RGOSMapProjection()
{
}

bool RGOSMapProjection::isValid() const
{
    //TODO: return m_bounds.isValid();
    return true;
}

QPoint RGOSMapProjection::convert(const QGeoCoordinate& geoPoint) const
{
    // Convert center latitude/longitude to tile coordinates
    QPointF centerTile = latLonToTile(m_osmBoumds.getCenterCoord(), m_osmBoumds.getZoom());

    // Convert current latitude/longitude to tile coordinates
    QPointF tile = latLonToTile(geoPoint, m_osmBoumds.getZoom());

    // Calculate pixel offsets relative to the center of the map
    double pixelX = (tile.x() - centerTile.x()) * TILE_SIZE + m_mapWidth / 2.0;
    double pixelY = (tile.y() - centerTile.y()) * TILE_SIZE + m_mapHeight / 2.0;

    return QPointF(pixelX, pixelY).toPoint();
}

bool RGOSMapProjection::saveProjection(const QString& fileName)
{
    //TODO: Save OSM Geo projection parameters
    //RGSettings::setMapGeoBounds(fileName, m_bounds);
    return true;
}

QPointF RGOSMapProjection::latLonToTile(const QGeoCoordinate& geoPoint, int zoom)
{
    double x = (geoPoint.longitude() + 180.0) / 360.0 * (1 << zoom);
    double y = (1.0 - log(tan(geoPoint.latitude() * M_PI / 180.0) + 1.0 / cos(geoPoint.latitude() * M_PI / 180.0)) / M_PI) / 2.0 * (1 << zoom);
    return QPointF(x, y);
}

QGeoCoordinate RGOSMapProjection::tileToLatLon(const QPointF& tileXY, int zoom)
{
    QGeoCoordinate geoCoord;
    double n = std::pow(2.0, zoom);
    geoCoord.setLongitude(tileXY.x() / n * 360.0 - 180.0);
    geoCoord.setLatitude(std::atan(std::sinh(M_PI * (1 - 2 * tileXY.y() / n))) * 180.0 / M_PI);
    return geoCoord;
}
