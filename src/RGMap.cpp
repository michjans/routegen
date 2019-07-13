#include "RGMap.h"

RGMap::RGMap(QObject *parent)
    :QObject(parent)
{

}

bool RGMap::loadMap(const QString &fileName, const QPixmap &map, const QRectF mapBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mGeoBounds = mapBounds;
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

bool RGMap::hasGeoBounds() const
{
    return mGeoBounds.isValid();
}

QList<QPoint> RGMap::mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const
{
    QList<QPoint> pointlist;
    if (mGeoBounds.isValid())
    {
        double xScale = mMap.width() / mGeoBounds.width();
        double yScale = mMap.height() / mGeoBounds.height();

        for (const QGeoCoordinate &coord: geoCoordinates)
        {
            QPoint point((coord.longitude() - mGeoBounds.x()) * xScale,
                         (mGeoBounds.y() - coord.latitude()) * yScale);
            pointlist.append(point);
        }
    }
    return pointlist;
}

QString RGMap::fileName() const
{
    return mFileName;
}
