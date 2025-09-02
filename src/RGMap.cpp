#include "RGMap.h"
#include "RGSettings.h"

#include "RGGeoTiffMapProjection.h"
#include "RGWebMercatorProjection.h"

#include <QDebug>
#include <QJsonObject>

RGMap::RGMap(QObject* parent)
    : QObject(parent),
      mDirty(false)
{
}

bool RGMap::loadMap(const QString& fileName, const QPixmap& map)
{
    bool success = !map.isNull();
    if (map.isNull())
    {
        mFileName = fileName;
        success = mMap.load(fileName);
    }
    else
    {
        mFileName = fileName;
        mMap = map;
    }

    if (success)
    {
        //TODO: It coulds also be a map imported from OSM?
        //      Store tags in image file, instead of separately in RGSettings?
        //      Then we can determine which kind of map bounds to load.
        //Store or retrieve google map's geo boundaries
        RGGoogleMapBounds googleBounds = RGSettings::getMapGeoBounds(fileName);

        if (googleBounds.isValid())
        {
            mMapProjection = std::make_unique<RGWebMercatorProjection>(googleBounds);
        }
        else if (fileName.endsWith(QLatin1String(".tif")))
        {
            //This is potentially a geotiff file
            mMapProjection = std::make_unique<RGGeoTiffMapProjection>(fileName);
        }

        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

bool RGMap::loadMap(const QString& fileName, const QPixmap& map, const RGGoogleMapBounds& gmapBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mMap = map;

    if (success)
    {
        if (gmapBounds.isValid())
        {
            //Store google map's geo boundaries
            mMapProjection = std::make_unique<RGWebMercatorProjection>(gmapBounds);
            mMapProjection->saveProjection(fileName);
        }

        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

bool RGMap::loadMap(const QString& fileName, const QPixmap& map, const RGOsMapBounds& osmBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mMap = map;

    if (success)
    {
        if (osmBounds.isValid())
        {
            //TODO: Since we now only use RGWebMercatorProjection we can store the geo bounds in the same
            //      way as the Google map bounds, but preferrable we now store them as meta-tags in the file.
            mMapProjection = std::make_unique<RGWebMercatorProjection>(osmBounds, mMap.width(), mMap.height());
            mMapProjection->saveProjection(fileName);
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

QList<QPoint> RGMap::mapRoutePoints(const QList<QGeoCoordinate>& geoCoordinates) const
{
    QList<QPoint> pointlist;
    if (mMapProjection)
    {
        for (const QGeoCoordinate& coord : geoCoordinates)
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

bool RGMap::saveGeoBoundsToNewFile(const QString& fileName)
{
    if (mMapProjection)
    {
        return mMapProjection->saveProjection(fileName);
    }
    return false;
}

void RGMap::read(const QJsonObject& json)
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

void RGMap::write(QJsonObject& json)
{
    QJsonObject mapObject;
    mapObject.insert(QStringLiteral("fileName"), mFileName);

    //The geobounds are stored along with the map in the settings, so no meed to store them in the project

    json.insert(QStringLiteral("map"), mapObject);
}
