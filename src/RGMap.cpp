#include "RGMap.h"
#include "RGSettings.h"

#include "RGGeoTiffMapProjection.h"
#include "RGWebMercatorProjection.h"

#include <QDebug>
#include <QImageWriter>
#include <QJsonObject>

namespace
{
//Keys used to store the georeferencing metadata into the image files
const QLatin1StringView imgKeyPixelToLeftXStr("RGWebMercatorPixelTopLeftX");
const QLatin1StringView imgKeyPixelToLeftYStr("RGWebMercatorPixelTopLeftY");
const QLatin1StringView imgKeyZoomStr("RGWebMercatorZoom");
} // namespace

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
        if (fileName.endsWith(QStringLiteral(".tif")))
        {
            //This is potentially a geotiff file
            mMapProjection = std::make_unique<RGGeoTiffMapProjection>(fileName);
            qDebug() << "GeoTiff file? RGGeoTiffMapProjection reports valid = " << mMapProjection->isValid();
        }
        else
        {
            //Try to initialize RGWebMercatorProjection from image.
            QString imgPixelToLeftXStr = map.text(imgKeyPixelToLeftXStr);
            QString imgPixelToLeftYStr = map.text(imgKeyPixelToLeftYStr);
            QString imgZoomStr = map.text(imgKeyZoomStr);
            if (!imgPixelToLeftXStr.isEmpty() && !imgPixelToLeftYStr.isEmpty() && !imgZoomStr.isEmpty())
            {
                qDebug() << "Georeference keys read from file:";
                qDebug() << "imgPixelToLeftXStr:" << imgPixelToLeftXStr;
                qDebug() << "imgPixelToLeftYStr:" << imgPixelToLeftYStr;
                qDebug() << "imgZoomStr:" << imgZoomStr;

                mMapProjection = std::make_unique<RGWebMercatorProjection>(QPoint(imgPixelToLeftXStr.toInt(), imgPixelToLeftYStr.toInt()), imgZoomStr.toInt(),
                                                                           map.width(), map.height());
                if (!mMapProjection->isValid())
                {
                    mMapProjection.release();
                    //For backward compatiblity we can still try to retrieve the RGGoogleMapBounds
                    //from the settings, but the new method is to store georeference tags in image file.
                    RGGoogleMapBounds googleBounds = RGSettings::getMapGeoBounds(fileName);
                    if (googleBounds.isValid())
                    {
                        mMapProjection = std::make_unique<RGWebMercatorProjection>(googleBounds, map.width(), map.height());
                    }
                }
            }
        }

        mMap = QPixmap::fromImage(map);
        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

template <typename MAPBOUNDS>
bool RGMap::saveImportedMapAndUse(const QString& fileName, const QPixmap& map, const MAPBOUNDS& mapBounds, bool& geoReferenceSavedSuccess)
{
    bool success = !map.isNull();
    geoReferenceSavedSuccess = false;
    mFileName = fileName;
    mMap = map;

    if (success)
    {
        //All imported maps use WebMercator projection
        mMapProjection = std::make_unique<RGWebMercatorProjection>(mapBounds, mMap.width(), mMap.height());
        if (fileName.endsWith(QStringLiteral(".tif")))
        {
            success = saveImageAndGeoTiffProjection(fileName, map.toImage(), geoReferenceSavedSuccess);
        }
        else
        {
            success = saveImageAndWebMercatorProjection(fileName, map.toImage(), geoReferenceSavedSuccess);
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

bool RGMap::saveGeoBoundsToNewFile(const QString& fileName /*TODO:, QImage &srcImage*/)
{
    //TODO:1: Here we should pass the QImage to which the geo reference information should be added using the QImageWriter,
    //        but take into account if we save as a tif file vs saving to png, jpg, etc. because then a different projection
    //        has to be used (e.g. web mercator vs geotiff)
    //        For now we only support writing webmercator, as that will be the most common way. Ideally we don't write into
    //        a new QImage, but pass it along from the caller, but this will require some refactoring.
    bool success = false, geoReferenceSavedSuccess = false;
    if (mMapProjection)
    {
        //TODO:2: This is still clumsy, because this class requires all this knowledge about used file extenstions, etc. and
        //        based on that use dynamic_cast to in order to call the right methods on the projection classes.
        //        We should refactor this, but for now just leave it as it is, so my weekend will not be occupied again.
        bool sourceWebMercator = !mFileName.endsWith(QStringLiteral("tif"));
        bool targetWebMercator = !fileName.endsWith(QStringLiteral("tif"));
        bool targetGeoTiff = !targetWebMercator;
        bool sourceGeotiff = !sourceWebMercator;

        QImage srcImage(fileName);
        if (sourceWebMercator && targetWebMercator)
        {
            success = saveImageAndWebMercatorProjection(fileName, srcImage, geoReferenceSavedSuccess);
        }
        else if (sourceWebMercator && targetGeoTiff)
        {
            success = saveImageAndGeoTiffProjection(fileName, srcImage, geoReferenceSavedSuccess);
        }
        else if (sourceGeotiff && targetGeoTiff)
        {
            success = saveGeoTiffToGeoTiffProjection(fileName, srcImage, geoReferenceSavedSuccess);
        }
        else
        {
            qWarning() << "We can't safe geotiff files back to web mercator files (non geotiff) image files";
        }
    }
    return success && geoReferenceSavedSuccess;
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

bool RGMap::saveImageAndWebMercatorProjection(const QString& fileName, const QImage& srcImage, bool& geoReferenceSavedSuccess)
{
    QImageWriter writer(fileName);
    if (writer.supportsOption(QImageIOHandler::Description) && mMapProjection->isValid())
    {
        //We store the geo refence information as tags in the file (if possible)
        //It's only required to store the topleft coordinate and the zoomlevel
        RGWebMercatorProjection* wmProjection = qobject_cast<RGWebMercatorProjection*>(mMapProjection.get());
        if (wmProjection)
        {
            QPoint topLeft = wmProjection->topLeftWorldPixel();
            writer.setText(imgKeyPixelToLeftXStr, QString::number(topLeft.x()));
            writer.setText(imgKeyPixelToLeftYStr, QString::number(topLeft.y()));
            writer.setText(imgKeyZoomStr, QString::number(wmProjection->zoomLevel()));
            geoReferenceSavedSuccess = true;
        }
    }
    else
    {
        qWarning() << "Image format for " << fileName << "does not support metadata to store georef tags";
    }
    //Now save the full image
    return writer.write(srcImage);
}

bool RGMap::saveImageAndGeoTiffProjection(const QString& fileName, const QImage& srcImage, bool& geoReferenceSavedSuccess)
{
    bool success = false;
    QImageWriter writer(fileName);

    //First save the tif file itself, then append the Geo keys
    success = writer.write(srcImage);
    if (success)
    {
        geoReferenceSavedSuccess = RGGeoTiffMapProjection::saveProjectionDataToGeoTiff(fileName, *(mMapProjection.get()));
        qDebug() << "Saving TIF file success = " << success << "; geoReferenceSavedSuccess = " << geoReferenceSavedSuccess;
    }
    else
    {
        qWarning() << "Cannot save " << fileName << "while trying to save image to GeoTiff format!";
    }
    return success;
}

bool RGMap::saveGeoTiffToGeoTiffProjection(const QString& fileName, const QImage& srcImage, bool& geoReferenceSavedSuccess)
{
    bool success = false;
    QImageWriter writer(fileName);

    //First save the tif file itself, then append the Geo keys
    success = writer.write(srcImage);
    if (success)
    {
        RGGeoTiffMapProjection* geoTiffProjection = qobject_cast<RGGeoTiffMapProjection*>(mMapProjection.get());
        if (geoTiffProjection)
        {
            geoReferenceSavedSuccess = geoTiffProjection->saveProjection(fileName);
        }
        qDebug() << "Saving TIF file success = " << success << "; geoReferenceSavedSuccess = " << geoReferenceSavedSuccess;
    }
    else
    {
        qWarning() << "Cannot save " << fileName << "while trying to save GeoTiff image to GeoTiff format!";
    }
    return success;
}

// Explicit instantiation of loadImportedMap for the possible variants
template bool RGMap::saveImportedMapAndUse<RGGoogleMapBounds>(const QString&, const QPixmap&, const RGGoogleMapBounds&, bool& geoReferenceSavedSuccess);
template bool RGMap::saveImportedMapAndUse<RGOsMapBounds>(const QString&, const QPixmap&, const RGOsMapBounds&, bool& geoReferenceSavedSuccess);
