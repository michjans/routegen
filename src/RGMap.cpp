#include "RGMap.h"
#include "RGSettings.h"

RGMap::RGMap(QObject *parent)
    :QObject(parent)
{

}

bool RGMap::loadMap(const QString &fileName, const QPixmap &map, const QRectF mapBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
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
        //Store or retrieve map's geo boundaries
        if (mapBounds.isValid())
        {
            mGeoBounds = mapBounds;
            RGSettings::setMapGeoBounds(fileName, mapBounds);
        }
        else
        {
            mGeoBounds = RGSettings::getMapGeoBounds(fileName);
        }

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

void RGMap::read(const QJsonObject &json)
{

}

void RGMap::write(QJsonObject &json)
{

}
