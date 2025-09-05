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

bool RGMap::loadMap(const QString& fileName, QImage map)
{
    bool success = !map.isNull();
    if (map.isNull())
    {
        mFileName = fileName;
        success = map.load(fileName);
    }
    else
    {
        mFileName = fileName;
    }

    if (success)
    {
        if (fileName.endsWith(QLatin1String(".tif")))
        {
            //This is potentially a geotiff file
            mMapProjection = std::make_unique<RGGeoTiffMapProjection>(fileName);
        }
        else
        {
            //TODO:Initialize RGWebMercatorProjection from image.
            qDebug() << "MAP KEYS:" << map.textKeys();

            //For backward compatiblity we can still retrieve the RGGoogleMapBounds
            //from the settings, but the new method is to store "geotags" in image file.
            RGGoogleMapBounds googleBounds = RGSettings::getMapGeoBounds(fileName);
            if (googleBounds.isValid())
            {
                mMapProjection = std::make_unique<RGWebMercatorProjection>(googleBounds);
            }
        }

        mMap = QPixmap::fromImage(map);
        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

bool RGMap::loadImportedMap(const QString& fileName, const QPixmap& map, const RGGoogleMapBounds& gmapBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mMap = map;

    if (success)
    {
        if (gmapBounds.isValid())
        {
            //TODO: If the map was saved as tif file, save using the RGGeoTiffMapProjection class, because when
            //      loading a tif file, we also use the RGGeoTiffMapProjection.
            //TODO: If we extend the RGWebMercatorProjection constructor to also accept map with/height, we can
            //      make this a template class with mapBounds as template parameter
            //Store google map's geo boundaries
            mMapProjection = std::make_unique<RGWebMercatorProjection>(gmapBounds);
            saveProjection(fileName);
        }

        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

bool RGMap::loadImportedMap(const QString& fileName, const QPixmap& map, const RGOsMapBounds& osmBounds)
{
    bool success = !map.isNull();
    mFileName = fileName;
    mMap = map;

    if (success)
    {
        if (osmBounds.isValid())
        {
            mMapProjection = std::make_unique<RGWebMercatorProjection>(osmBounds, mMap.width(), mMap.height());
            saveProjection(fileName);
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
        return saveProjection(fileName);
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

bool RGMap::saveProjection(const QString& fileName)
{
    //TODO: Do a check on file extenstion and depending on that store the geo reference information in a different
    //      way. E.g. if TIFF, use GeoTiff, if PNG, GIF, use RGWebMercatorProjection
    //      The current mMapProjection should get methods to return the worldOriginXY coordinate and all required
    //      information.
    //      Add static methods to both projection classes to store the projection data from another projection system.
    //      E.g. RGeoTiffMapProjection::saveProjection(const RGMapProjection &srcProjection);
    //           RGWebMercatorProjection::saveProjection(const RGMapProjection &srcProjection);
    //      ...Hmmm, this is not entirely correct because the projection system is independent of the way that the
    //         georefernce data is stored....
    //         Maybe we should create a RGGeoReferenceWriter that can store in the correct format.

    return mMapProjection->saveProjection(fileName);
}
