#include "RGGeoTiffMapProjection.h"

#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>

//GeoTiff headers
#include "geo_normalize.h"
#include "geovalues.h"
#include "xtiffio.h"

RGGeoTiffMapProjection::RGGeoTiffMapProjection(const QString& fileName, QObject* parent)
    : RGMapProjection(parent),
      mFileName(fileName),
      mTiff(nullptr),
      mGTif(nullptr)
{
    std::string fileNameStr = fileName.toStdString();
    mTiff = XTIFFOpen(fileNameStr.c_str(), "r");
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
    GTIFDefn defn;
    if (GTIFGetDefn(mGTif, &defn))
    {
        qDebug() << "defn.Model=" << defn.Model;

        int xsize, ysize;
        TIFFGetField(mTiff, TIFFTAG_IMAGEWIDTH, &xsize);
        TIFFGetField(mTiff, TIFFTAG_IMAGELENGTH, &ysize);

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
        if (GTIFImageToPCS(mGTif, &lon, &lat))
        {
            qDebug() << "SW lat:" << lat;
            qDebug() << "SW lon:" << lon;
        }
        lon = xmax;
        lat = ymin;
        if (GTIFImageToPCS(mGTif, &lon, &lat))
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

QPoint RGGeoTiffMapProjection::convert(const QGeoCoordinate& geoPoint) const
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

bool RGGeoTiffMapProjection::saveProjection(const QString& fileName)
{
    //Write the projection data to the TIFF (fileName) that was already
    //created, but still without the GeoGraphic tags.
    //TODO: Still finding a way to directly use the geotiff API, but for now
    //      just use the tools provided with libgeotiff
    QDir tmpDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    if (!tmpDir.exists())
    {
        //Shoudn't happen
        return false;
    }

    if (QFileInfo(fileName).suffix() != QLatin1String("tif"))
    {
        qDebug() << "GeoTIFF only supported by tif files";
        return false;
    }

    std::unique_ptr<QProcess> listGeoProcess = std::make_unique<QProcess>();
    QStringList arguments;
    arguments << mFileName;
    listGeoProcess->start(QStringLiteral("listgeo"), arguments);
    if (!listGeoProcess->waitForFinished(5000))
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to execute listgeo, is executable available?"));
        return false;
    }
    QByteArray output = listGeoProcess->readAllStandardOutput();
    QFile geoFile(tmpDir.absolutePath() + "/" + QFileInfo(mFileName).baseName() + ".geo");
    if (geoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        geoFile.write(output);
        geoFile.close();
    }
    else
    {
        QMessageBox::critical(nullptr, tr("Error"), tr("Unable to write %1! Disk full or no permissions?").arg(geoFile.fileName()));
        return false;
    }

    std::unique_ptr<QProcess> applyGeoProcess = std::make_unique<QProcess>();
    arguments.clear();
    arguments << geoFile.fileName() << fileName;
    applyGeoProcess->start(QStringLiteral("applygeo"), arguments);
    if (!applyGeoProcess->waitForFinished())
    {
        QMessageBox::critical(nullptr, tr("Error"), QString(tr("Unable to execute applygeo, is executable available?")));
        return false;
    }

    return true;
}

bool RGGeoTiffMapProjection::saveProjectionDataToGeoTiff(const QString& fileName, const RGMapProjection& rgMapProj)
{
    std::string fileNameStr = fileName.toStdString();
    TIFF* tif = XTIFFOpen(fileNameStr.c_str(), "r+");
    if (!tif)
    {
        return false;
    }

    GTIF* gtif = GTIFNew(tif);
    if (!gtif)
    {
        qDebug() << "Failed opening GTIF.";
        XTIFFClose(tif);
        return false;
    }
    double x_origin, y_origin, pixel_size;
    int csType;

    bool success = false;
    if (rgMapProj.fillProjectionTransformation(x_origin, y_origin, pixel_size, csType))
    {
        GTIFKeySet(gtif, GTModelTypeGeoKey, TYPE_SHORT, 1, ModelTypeProjected);
        GTIFKeySet(gtif, GTRasterTypeGeoKey, TYPE_SHORT, 1, RasterPixelIsArea);
        GTIFKeySet(gtif, ProjectedCSTypeGeoKey, TYPE_SHORT, 1, csType);

        // tiepoint: image (0,0) maps to projected (x_origin, y_origin)
        double tiepoints[6] = {0, 0, 0, x_origin, y_origin, 0};
        TIFFSetField(tif, TIFFTAG_GEOTIEPOINTS, 6, tiepoints);

        // pixel size
        double pixsize[3] = {pixel_size, -pixel_size, 0};
        TIFFSetField(tif, TIFFTAG_GEOPIXELSCALE, 3, pixsize);

        GTIFWriteKeys(gtif);
        success = true;
    }
    GTIFFree(gtif);
    XTIFFClose(tif);
    return success;
}
