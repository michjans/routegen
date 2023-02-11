#include "RGGoogleMapProjection.h"
#include "RGSettings.h"

#include <QtMath>
#include <QDebug>

namespace
{
    const int TILE_SIZE = 256;
}

RGGoogleMapProjection::RGGoogleMapProjection(const RGMapBounds &mapBounds)
    : m_bounds(mapBounds)
{
    //Calculate topleft/bottomright of the current map in pixel coordinates
    mTopLeft = worldToPixel(project(QGeoCoordinate(m_bounds.getNE().latitude(), m_bounds.getSW().longitude())));
    mBottomRight = worldToPixel(project(QGeoCoordinate(m_bounds.getSW().latitude(), m_bounds.getNE().longitude())));
    mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();
    qDebug() << "zoom:" << m_bounds.getZoom();
    qDebug() << "pixWidth:" << mBottomRight.x() - mTopLeft.x();
    qDebug() << "pixHeigth:" << mBottomRight.y() - mTopLeft.y();
}

RGGoogleMapProjection::~RGGoogleMapProjection()
{

}

bool RGGoogleMapProjection::isValid() const
{
    return m_bounds.isValid();
}

QPoint RGGoogleMapProjection::convert(const QGeoCoordinate &geoPoint) const
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

void RGGoogleMapProjection::saveProjection(const QString &fileName)
{
    //If current map has geo bounds, also store geo bounds along with the saved map image
    RGSettings::setMapGeoBounds(fileName, m_bounds);
}

QPointF RGGoogleMapProjection::project(const QGeoCoordinate &geoPoint) const
{
    qreal siny = qSin(geoPoint.latitude() * M_PI / 180);

    // Algorithm copied from Google maps API
    // Truncating to 0.9999 effectively limits latitude to 89.189. This is
    // about a third of a tile past the edge of the world tile.
    siny = std::min(std::max(siny, -0.9999), 0.9999);

    return QPointF(
        TILE_SIZE * (0.5 + geoPoint.longitude() / 360),
        TILE_SIZE * (0.5 - std::log((1 + siny) / (1 - siny)) / (4 * M_PI)));
}

QPoint RGGoogleMapProjection::worldToPixel(const QPointF &worldPoint) const
{
    int scale = 1 << m_bounds.getZoom();

    return QPoint(qFloor(worldPoint.x() * scale), qFloor(worldPoint.y() * scale));
}
