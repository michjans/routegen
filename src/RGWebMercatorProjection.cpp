#include "RGWebMercatorProjection.h"

#include <QDebug>
#include <QImage>
#include <QtMath>

namespace
{
const int TILE_SIZE = 256;

//Keys used to store the georeferencing metadata into the image files
const QLatin1StringView imgKeyPixelToLeftXStr("RGWebMercatorPixelTopLeftX");
const QLatin1StringView imgKeyPixelToLeftYStr("RGWebMercatorPixelTopLeftY");
const QLatin1StringView imgKeyZoomStr("RGWebMercatorZoom");
}

RGWebMercatorProjection::RGWebMercatorProjection(const RGGoogleMapBounds& mapBounds, QObject* parent)
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
    //We store the geo refence information as tags in the file (if possible)
    //It's only required to store the topleft coordinate and the zoomlevel
    QImage img(fileName);
    img.setText(imgKeyPixelToLeftXStr, QString::number(mTopLeft.x()));
    img.setText(imgKeyPixelToLeftYStr, QString::number(mTopLeft.y()));
    img.setText(imgKeyZoomStr, QString::number(m_zoom));
    return img.save(fileName);
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
