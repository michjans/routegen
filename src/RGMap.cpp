#include "RGMap.h"
#include "RGSettings.h"

#include "RGGeoTiffMapProjection.h"
#include "RGGoogleMapProjection.h"
#include "RGOSMapProjection.h"
#include "RGWebMercatorProjection.h"

#include <QDebug>
#include <QJsonObject>

RGMap::RGMap(QObject* parent)
    : QObject(parent),
      mDirty(false)
{
}

bool RGMap::loadMap(const QString& fileName)
{
    mFileName = fileName;
    bool success = mMap.load(fileName);

    if (success)
    {
        //TODO: It coulds also be a map imported from OSM?
        //      Store tags in image file, instead of separately in RGSettings?
        //      Then we can determine which kind of map bounds to load.
        //Store or retrieve google map's geo boundaries
        RGGoogleMapBounds googleBounds = RGSettings::getMapGeoBounds(fileName);

        if (googleBounds.isValid())
        {
            mMapProjection = std::make_unique<RGGoogleMapProjection>(googleBounds);
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

bool RGMap::loadMap(const QString& fileName, const QPixmap& map)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mMap = map;

    //TODO: The block below is identical as in method above, so put in commont function!
    if (success)
    {
        //TODO: It coulds also be a map imported from OSM?
        //      Store tags in image file, instead of separately in RGSettings?
        //      Then we can determine which kind of map bounds to load.
        //Store or retrieve google map's geo boundaries
        RGGoogleMapBounds googleBounds = RGSettings::getMapGeoBounds(fileName);

        if (googleBounds.isValid())
        {
            mMapProjection = std::make_unique<RGGoogleMapProjection>(googleBounds);
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
            RGSettings::setMapGeoBounds(fileName, gmapBounds);
            mMapProjection = std::make_unique<RGGoogleMapProjection>(gmapBounds);
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
            //TODO: Find a way to store OSM map's geo boundaries, e.g. as meta tags in file
            //RGSettings::setMapGeoBounds(fileName, gmapBounds);
            //mMapProjection = std::make_unique<RGOSMapProjection>(osmBounds, mMap.width(), mMap.height());
            mMapProjection = std::make_unique<RGWebMercatorProjection>(osmBounds, mMap.width(), mMap.height());
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
