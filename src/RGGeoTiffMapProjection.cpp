/*
    Copyright (C) 2008-2025  Michiel Jansen

This file is part of Route Generator.

Route Generator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "RGGeoTiffMapProjection.h"

#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <cmath>

// GeoTiff headers
#include <geo_normalize.h>
#include <geovalues.h>
#include <xtiffio.h>

namespace
{
// Web Mercator constants
static constexpr double kEarthRadius = 6378137.0; // meters (WGS84)

void wgs84ToWebMercator(double lonDeg, double latDeg, double& x, double& y)
{
    // lonDeg, latDeg in degrees -> EPSG:3857 meters
    const double lonRad = lonDeg * M_PI / 180.0;
    const double latRad = latDeg * M_PI / 180.0;
    x = kEarthRadius * lonRad;
    // clamp latitude to valid Mercator range to avoid Inf
    const double maxLat = 85.05112877980659; // approx
    double latC = std::min(std::max(latDeg, -maxLat), maxLat);
    double latCRad = latC * M_PI / 180.0;
    y = kEarthRadius * std::log(std::tan(M_PI / 4.0 + latCRad / 2.0));
}
} // namespace

RGGeoTiffMapProjection::RGGeoTiffMapProjection(const QString& fileName, QObject* parent)
    : RGMapProjection(parent),
      mFileName(fileName),
      mTiff(nullptr),
      mGTif(nullptr),
      mModelType(-1)
{
    std::string fileNameStr = fileName.toStdString();
    mTiff = XTIFFOpen(fileNameStr.c_str(), "r");
    if (!mTiff)
    {
        qDebug() << "RGGeoTiffMapProjection: failed to open TIFF:" << fileName;
        return;
    }

    mGTif = GTIFNew(mTiff);
    if (!mGTif)
    {
        qDebug() << "RGGeoTiffMapProjection: Failed opening GTIF.";
        cleanup();
        return;
    }

    GTIFDefn defn;
    if (!GTIFGetDefn(mGTif, &defn))
    {
        qDebug() << "RGGeoTiffMapProjection: no GeoTIFF definition present.";
        cleanup();
        return;
    }

    // Record model type
    mModelType = defn.Model;

    short projectedCSType = 0;
    if (mModelType == ModelTypeProjected)
    {
        // Try to read ProjectedCSTypeGeoKey (epsg) from the file. There are diffeent methods in how this can be stored
        // depending on the program that created it, so we support (at least) two, which is hopefully sufficient for
        // the use case of Route Generator.
        unsigned short pcs = 0;
        if (GTIFKeyGetSHORT(mGTif, ProjectedCSTypeGeoKey, &pcs, 0, 1) == 1)
        {
            projectedCSType = pcs;
        }
        else if (defn.ProjCode != 0)
        {
            // defn.ProjCode may sometimes contain EPSG code
            projectedCSType = defn.ProjCode;
        }
        // Only for EPSG:3857 (Pseudo-Mercator) we have added a wgs84ToWebMercator projection conversion.
        // To support any projection method we have to become dependent of GDAL...
        if (projectedCSType != 3857)
        {
            qWarning() << "Projection type" << projectedCSType << "not supported, supporting 3857 only (Pseudo-Mercator)";
            cleanup();
            return;
        }
    }
    else if (mModelType != ModelTypeGeographic)
    {
        //For ModelTypeGeographic no projection conversion is required, so we can call GTIFPCSToImage directly
        qWarning() << "Model type " << mModelType << "not supported";
        cleanup();
        return;
    }

#ifndef QT_NO_DEBUG_OUTPUT
    // log extents (pixel -> PCS) for debugging
    int xsize = 0, ysize = 0;
    TIFFGetField(mTiff, TIFFTAG_IMAGEWIDTH, &xsize);
    TIFFGetField(mTiff, TIFFTAG_IMAGELENGTH, &ysize);

    qDebug() << "RGGeoTiffMapProjection: xsize=" << xsize << " ysize=" << ysize;
    qDebug() << "RGGeoTiffMapProjection: ModelType=" << mModelType << " ProjectedCSType=" << projectedCSType;
#endif
}

RGGeoTiffMapProjection::~RGGeoTiffMapProjection()
{
    cleanup();
}

bool RGGeoTiffMapProjection::isValid() const
{
    // valid only if TIFF/GTIF opened *and* model is one we recognize and can use
    return (mTiff && mGTif && (mModelType == ModelTypeGeographic || mModelType == ModelTypeProjected));
}

QPoint RGGeoTiffMapProjection::convert(const QGeoCoordinate& geoPoint) const
{
    QPoint point;
    if (!isValid())
        return point;

    // Determine PCS input coordinates expected by GTIFPCSToImage.
    double pcsX = 0.0;
    double pcsY = 0.0;

    if (mModelType == ModelTypeGeographic)
    {
        // GeoTIFF expects lon/lat degrees
        pcsX = geoPoint.longitude();
        pcsY = geoPoint.latitude();
    }
    else if (mModelType == ModelTypeProjected)
    {
        // For projected images can only handle EPSG:3857 (Web Mercator) here, but this is already checked in the constructor
        // First convert lon/lat -> projected units (meters) for Web Mercator
        wgs84ToWebMercator(geoPoint.longitude(), geoPoint.latitude(), pcsX, pcsY);
    }
    //else: should not occur, because the invalid check already covers this situation

    double xi = pcsX;
    double yi = pcsY;
    qDebug() << "xi, yi=" << xi << "," << yi;
    if (GTIFPCSToImage(mGTif, &xi, &yi))
    {
        // GTIFPCSToImage writes the image (pixel) coords back into xi, yi
        // xi/yi are doubles but QPoint takes ints; you can round as needed
        point.setX(static_cast<int>(std::round(xi)));
        point.setY(static_cast<int>(std::round(yi)));
        qDebug() << "point=" << point;
    }
    else
    {
        qWarning() << "RGGeoTiffMapProjection::convert: GTIFPCSToImage failed for PCS coords:" << pcsX << pcsY;
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
        qWarning() << "Failed opening XTIF:" << fileName;
        return false;
    }

    GTIF* gtif = GTIFNew(tif);
    if (!gtif)
    {
        qWarning() << "Failed opening GTIF:" << fileName;
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
    else
    {
        qWarning() << "Unable to fill projection transformation for file:" << fileName;
    }
    GTIFFree(gtif);
    XTIFFClose(tif);
    return success;
}

void RGGeoTiffMapProjection::cleanup()
{
    if (mGTif)
    {
        GTIFFree(mGTif);
        mGTif = nullptr;
    }
    if (mTiff)
    {
        XTIFFClose(mTiff);
        mTiff = nullptr;
    }
}
