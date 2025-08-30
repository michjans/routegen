#include "RGWebMercatorProjection.h"
#include "RGSettings.h"

#include <QDebug>
#include <QtMath>

namespace
{
const int TILE_SIZE = 256;
}

RGWebMercatorProjection::RGWebMercatorProjection(const RGGoogleMapBounds& mapBounds, QObject* parent)
    : RGMapProjection(parent),
      m_bounds(mapBounds)
{
    //Calculate topleft/bottomright of the current map in pixel coordinates
    mTopLeft = worldToPixel(project(QGeoCoordinate(m_bounds.getNE().latitude(), m_bounds.getSW().longitude())));
    mBottomRight = worldToPixel(project(QGeoCoordinate(m_bounds.getSW().latitude(), m_bounds.getNE().longitude())));
    mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();
    qDebug() << "zoom:" << m_bounds.getZoom();
    qDebug() << "pixWidth:" << mBottomRight.x() - mTopLeft.x();
    qDebug() << "pixHeigth:" << mBottomRight.y() - mTopLeft.y();
}

RGWebMercatorProjection::RGWebMercatorProjection(const RGOsMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent)
    : RGMapProjection(parent),
      //TODO: worldToPixel requires the zoom to be set in m_bounds, but we should probably just store zoom as separate
      //      member, but for now I just wanted to have this working quickly
      m_bounds(QGeoCoordinate(), QGeoCoordinate(), mapBounds.getZoom())
{
    QPointF worldCenter = project(mapBounds.getCenterCoord());
    QPoint centerPixel = worldToPixel(worldCenter);

    mTopLeft = centerPixel - QPoint(mapWidth / 2, mapHeight / 2);
    mBottomRight = centerPixel + QPoint(mapWidth / 2, mapHeight / 2);
    mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();

    QGeoCoordinate sw = unproject(pixelToWorld(mBottomRight));
    QGeoCoordinate ne = unproject(pixelToWorld(mTopLeft));
    m_bounds = RGGoogleMapBounds(ne, sw, mapBounds.getZoom());
}

RGWebMercatorProjection::~RGWebMercatorProjection()
{
}

bool RGWebMercatorProjection::isValid() const
{
    return m_bounds.isValid();
}

QPoint RGWebMercatorProjection::convert(const QGeoCoordinate& geoPoint) const
{
    QPoint point = worldToPixel(project(geoPoint));
    if (mTopLeft.x() > mBottomRight.x() && point.x() < mTopLeft.x())
    {
        //When the route passes around the 180.0/-180.0 meridian (antimeridian), the coordinates will wrap around and we need to correct for this
        //by calculating the x pixel position of the antimeridan boundary and add that to all coordinates < the top left x value
        point.setX(mAntiMeredianPosX + point.x());
    }

    return point - mTopLeft;
}

QGeoCoordinate RGWebMercatorProjection::pixelToGeo(const QPoint& pixel) const
{
    // Re-add the top-left offset
    QPoint absPixel = pixel + mTopLeft;

    // If the map crosses the antimeridian, normalize the x coordinate
    if (mTopLeft.x() > mBottomRight.x() && absPixel.x() > mAntiMeredianPosX)
    {
        absPixel.setX(absPixel.x() - mAntiMeredianPosX);
    }

    QPointF world = pixelToWorld(absPixel);
    return unproject(world);
}

bool RGWebMercatorProjection::saveProjection(const QString& fileName)
{
    //If current map has geo bounds, also store geo bounds along with the saved map image
    RGSettings::setMapGeoBounds(fileName, m_bounds);
    return true;
}

QPointF RGWebMercatorProjection::project(const QGeoCoordinate& geoPoint) const
{
    qreal siny = qSin(geoPoint.latitude() * M_PI / 180);

    // Algorithm copied from Google maps API
    // Truncating to 0.9999 effectively limits latitude to 89.189. This is
    // about a third of a tile past the edge of the world tile.
    siny = std::min(std::max(siny, -0.9999), 0.9999);

    return QPointF(TILE_SIZE * (0.5 + geoPoint.longitude() / 360), TILE_SIZE * (0.5 - std::log((1 + siny) / (1 - siny)) / (4 * M_PI)));
}

QGeoCoordinate RGWebMercatorProjection::unproject(const QPointF& worldPoint) const
{
    double lon = (worldPoint.x() / TILE_SIZE - 0.5) * 360.0;

    double n = M_PI - 2.0 * M_PI * (worldPoint.y() / TILE_SIZE - 0.5);
    double lat = 180.0 / M_PI * std::atan(0.5 * (std::exp(n) - std::exp(-n)));

    return QGeoCoordinate(lat, lon);
}

QPoint RGWebMercatorProjection::worldToPixel(const QPointF& worldPoint) const
{
    int scale = 1 << m_bounds.getZoom();

    return QPoint(qFloor(worldPoint.x() * scale), qFloor(worldPoint.y() * scale));
}

QPointF RGWebMercatorProjection::pixelToWorld(const QPoint& pixel) const
{
    int scale = 1 << m_bounds.getZoom();
    return QPointF(static_cast<qreal>(pixel.x()) / scale, static_cast<qreal>(pixel.y()) / scale);
}
