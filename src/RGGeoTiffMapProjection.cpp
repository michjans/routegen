#include "RGGeoTiffMapProjection.h"

#include <QDebug>

//GeoTiff headers
#include "geo_normalize.h"
#include "geovalues.h"
#include "xtiffio.h"

RGGeoTiffMapProjection::RGGeoTiffMapProjection(const QString &fileName)
    : mTiff(nullptr),
      mGTif(nullptr)
{
    std::string fileNameStr = fileName.toStdString();
    mTiff = XTIFFOpen(fileNameStr.c_str(),"r");
    if (!mTiff)
    {
        return;
    }

    mGTif = GTIFNew(mTiff);
    if (!mGTif)
    {
        qDebug() << "Failed opening GTIF.";
        XTIFFClose(mTiff);
        mTiff = nullptr;
        return;
    }

    /* Get the GeoTIFF directory info */
    geocode_t model;    /* all key-codes are of this type */
    if (!GTIFKeyGet(mGTif, GTModelTypeGeoKey, &model, 0, 1))
    {
        qDebug() << "No Model Type";
        GTIFFree(mGTif);
        XTIFFClose(mTiff);
        mTiff = nullptr;
        mGTif = nullptr;
    }

    GTIFDefn	defn;
    if( GTIFGetDefn( mGTif, &defn ) )
    {
        int		xsize, ysize;
        TIFFGetField( mTiff, TIFFTAG_IMAGEWIDTH, &xsize );
        TIFFGetField( mTiff, TIFFTAG_IMAGELENGTH, &ysize );

        qDebug() << "xsize=" << xsize;
        qDebug() << "ysize=" << ysize;

        unsigned short raster_type = RasterPixelIsArea;
        GTIFKeyGetSHORT(mGTif, GTRasterTypeGeoKey, &raster_type, 0, 1);
        double xmin = (raster_type == RasterPixelIsArea) ? 0.0 : -0.5;
        double ymin = xmin;
        double ymax = ymin + ysize;
        double xmax = xmin + xsize;

        double lon = xmin;
        double lat = ymax;
        if( GTIFImageToPCS( mGTif, &lon, &lat ) )
        {
            qDebug() << "SW lat:" << lat;
            qDebug() << "SW lon:" << lon;
        }
        lon = xmax;
        lat = ymin;
        if( GTIFImageToPCS( mGTif, &lon, &lat ) )
        {
            qDebug() << "NE lat:" << lat;
            qDebug() << "NE lon:" << lon;
        }
    }
    else
    {
        //No valid Geo reference information found in GeoTIFF file
        GTIFFree(mGTif);
        XTIFFClose(mTiff);
        mTiff = nullptr;
        mGTif = nullptr;
    }
}

RGGeoTiffMapProjection::~RGGeoTiffMapProjection()
{
    GTIFFree(mGTif);
    XTIFFClose(mTiff);
}

bool RGGeoTiffMapProjection::isValid() const
{
    return mTiff && mGTif;
}

QPoint RGGeoTiffMapProjection::convert(const QGeoCoordinate &geoPoint) const
{
    QPoint point;
    if (isValid())
    {
        double x_io = geoPoint.longitude();
        double y_io = geoPoint.latitude();

        if (GTIFPCSToImage(mGTif, &x_io, &y_io))
        {
            point.setX(x_io);
            point.setY(y_io);
        }
    }

    return point;
}
