#include "RGMap.h"

RGMap::RGMap(QObject *parent)
    :QObject(parent)
{

}

bool RGMap::loadMap(const QString &fileName, const QPixmap &map, const QRectF mapBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mWorldBounds = mapBounds;
    if (map.isNull())
    {
        success = mMap.load(fileName);
    }
    else
    {
        mMap = map;
    }
    if (success)
    {
        emit mapLoaded(mMap);
    }
    return success;
}

bool RGMap::hasWorldBounds() const
{
    return mWorldBounds.isValid();
}

QList<QPoint> RGMap::mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const
{
    QList<QPoint> pointlist;
    if (mWorldBounds.isValid())
    {
        double xScale = mMap.width() / mWorldBounds.width();
        double yScale = mMap.height() / mWorldBounds.height();

        for (const QGeoCoordinate &coord: geoCoordinates)
        {
            QPoint point((coord.longitude() - mWorldBounds.x()) * xScale,
                         (mWorldBounds.y() - coord.latitude()) * yScale);
            pointlist.append(point);
        }
    }
    return pointlist;
}

QString RGMap::fileName() const
{
    return mFileName;
}
