#include "RGMap.h"
#include "RGSettings.h"

#include <QJsonObject>
#include <QtMath>
#include <QDebug>

//GeoTiff headers
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "xtiffio.h"

#include <algorithm>

namespace
{
    int TILE_SIZE = 256;
}

RGMap::RGMap(QObject *parent)
    :QObject(parent),
     mDirty(false)
{

}

bool RGMap::loadMap(const QString &fileName, const QPixmap &map, const RGMapBounds &mapBounds)
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
            m_bounds = mapBounds;
            RGSettings::setMapGeoBounds(fileName, mapBounds);
        }
        else
        {
            m_bounds = RGSettings::getMapGeoBounds(fileName);
        }

        if (!m_bounds.isValid() && fileName.endsWith(".tif"))
        {
            //Check if it's a geotiff file and import boundaries from file
            m_bounds = getMapBoundsFromGeoTiff(fileName);
        }

        if (m_bounds.isValid())
        {
            //TODO:At this point we can have a geotiff file or a google maps imported map.
            //     Either we should transfer everything to make use of geotiff (i.e. also store the imported google map in geotiff format),
            //     so we can use the geotiff lib coord conversion methods from now on.
            //     Or else make a separate conversion class for either geotiff or google maps project conversions and use that from now on.
            //Calculate topleft/bottomright of the current map in pixel coordinates
            mTopLeft = worldToPixel(project(QGeoCoordinate(m_bounds.getNE().latitude(), m_bounds.getSW().longitude())));
            mBottomRight = worldToPixel(project(QGeoCoordinate(m_bounds.getSW().latitude(), m_bounds.getNE().longitude())));
            mAntiMeredianPosX = worldToPixel(QPointF(TILE_SIZE, TILE_SIZE)).x();
            qDebug() << "zoom:" << m_bounds.getZoom();
            qDebug() << "pixWidth:" << mBottomRight.x() - mTopLeft.x();
            qDebug() << "pixHeigth:" << mBottomRight.y() - mTopLeft.y();
        }            
        else
        {
            mTopLeft = QPoint();
            mBottomRight = QPoint();
            mAntiMeredianPosX = 0.0;
        }

        emit mapLoaded(mMap);
    }

    mDirty = true;

    return success;
}

bool RGMap::hasGeoBounds() const
{
    return m_bounds.isValid();
}

RGMapBounds RGMap::geoBounds() const
{
    return m_bounds;
}

QList<QPoint> RGMap::mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const
{
    QList<QPoint> pointlist;
    if (m_bounds.isValid())
    {
        for (const QGeoCoordinate &coord: geoCoordinates)
        {
            QPoint point = worldToPixel(project(coord));
            if (mTopLeft.x() > mBottomRight.x() && point.x() < mTopLeft.x())
            {
                //When the route passes around the 180.0/-180.0 meridian (antimeridian), the coordinates will wrap around and we need to correct for this
                //by calculating the x pixel position of the antimeridan boundary and add that to all coordinates < the top left x value
                point.setX(mAntiMeredianPosX + point.x());
            }

            pointlist.append(point - mTopLeft);
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
    m_bounds = RGMapBounds();
    resetDirty();
    emit mapLoaded(mMap);
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

QPointF RGMap::project(const QGeoCoordinate &geoPoint) const
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

QPoint RGMap::worldToPixel(const QPointF &worldPoint) const
{
    int scale = 1 << m_bounds.getZoom();

    return QPoint(qFloor(worldPoint.x() * scale), qFloor(worldPoint.y() * scale));
}

RGMapBounds RGMap::getMapBoundsFromGeoTiff(const QString &fileName)
{
    RGMapBounds mapBounds;
    std::string fileNameStr = fileName.toStdString();
    TIFF *tif=XTIFFOpen(fileNameStr.c_str(),"r");
    if (!tif)
    {
        return mapBounds;
    }

    GTIF *gtif = GTIFNew(tif);
    if (!gtif)
    {
        qDebug() << "Failed opening GTIF.";

        XTIFFClose(tif);
        return mapBounds;
    }

    GTIFPrint(gtif,0,0);

    GTIFDefn	defn;
    if( GTIFGetDefn( gtif, &defn ) )
    {
        GTIFPrintDefnEx( gtif, &defn, stdout );

        int		xsize, ysize;
        TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &xsize );
        TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &ysize );
        unsigned short raster_type = RasterPixelIsArea;
        GTIFKeyGetSHORT(gtif, GTRasterTypeGeoKey, &raster_type, 0, 1);
        double xmin = (raster_type == RasterPixelIsArea) ? 0.0 : -0.5;
        double ymin = xmin;
        double ymax = ymin + ysize;
        double xmax = xmin + xsize;


        /*
        GTIFReportACorner( gtif, defn, fp_out, "Lower Left", xmin, ymax,
                           inv_flag, dec_flag );
        GTIFReportACorner( gtif, defn, fp_out, "Upper Right", xmax, ymin,
                           inv_flag, dec_flag );
        GTIFReportACorner( gtif, defn, fp_out, "Lower Right", xmax, ymax,
                           inv_flag, dec_flag );
        GTIFReportACorner( gtif, defn, fp_out, "Center", xmin + xsize/2.0, ymin + ysize/2.0,
                           inv_flag, dec_flag );
        */

        double lon = xmin;
        double lat = ymax;
        if( !GTIFImageToPCS( gtif, &lon, &lat ) )
        {
            qDebug() << "lat:" << lat;
            qDebug() << "lon:" << lon;
        }
    }

    GTIFFree(gtif);
    XTIFFClose(tif);
    return mapBounds;
}
