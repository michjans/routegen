#include "RGMap.h"
#include "RGSettings.h"

#include "RGGeoTiffMapProjection.h"
#include "RGGoogleMapProjection.h"

#include <QJsonObject>
#include <QDebug>

#include <algorithm>

RGMap::RGMap(QObject *parent)
    :QObject(parent),
     mDirty(false)
{

}

bool RGMap::loadMap(const QString &fileName, const QPixmap &map, const RGMapBounds &gmapBounds)
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
        //Store or retrieve google map's geo boundaries
        RGMapBounds googleBounds;
        if (gmapBounds.isValid())
        {
            //If gmapBounds is passed, we know it's an imported Google map
            googleBounds = gmapBounds;
            RGSettings::setMapGeoBounds(fileName, gmapBounds);
        }
        else
        {
            googleBounds = RGSettings::getMapGeoBounds(fileName);
        }

        if (googleBounds.isValid())
        {
            mMapProjection = std::make_unique<RGGoogleMapProjection>(googleBounds);
        }
        else if (fileName.endsWith(".tif"))
        {
            //This is potentially a geotiff file
            mMapProjection = std::make_unique<RGGeoTiffMapProjection>(fileName);
        }

        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

bool RGMap::hasGeoBounds() const
{
    return mMapProjection && mMapProjection->isValid();
}

QList<QPoint> RGMap::mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const
{
    QList<QPoint> pointlist;
    if (mMapProjection)
    {
        for (const QGeoCoordinate &coord: geoCoordinates)
        {
            pointlist.append(mMapProjection->convert(coord));
        }
    }
    return pointlist;
}

QString RGMap::fileName() const
{
    return mFileName;
}

bool RGMap::isEmpty() const
{
    return mMap.isNull();
}

bool RGMap::isDirty() const
{
    return mDirty;
}

void RGMap::resetDirty()
{
    mDirty = false;
}

void RGMap::clearMap()
{
    mMap = QPixmap();
    mMapProjection.release();
    resetDirty();
    emit mapLoaded(mMap);
}

bool RGMap::saveGeoBoundsToNewFile(const QString &fileName)
{
    if (mMapProjection)
    {
        return mMapProjection->saveProjection(fileName);
    }
    return false;
}

void RGMap::read(const QJsonObject &json)
{
    QJsonValue mapValue = json.value(QStringLiteral("map"));
    if (mapValue.isObject())
    {
        QJsonObject mapObject = mapValue.toObject();
        QString fileName = mapObject.value(QStringLiteral("fileName")).toString();
        if (!fileName.isEmpty())
        {
            loadMap(fileName);
        }
    }
}

void RGMap::write(QJsonObject &json)
{
    QJsonObject mapObject;
    mapObject.insert(QStringLiteral("fileName"), mFileName);

    //The geobounds are stored along with the map in the settings, so no meed to store them in the project

    json.insert(QStringLiteral("map"), mapObject);
}
