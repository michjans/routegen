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

#ifndef RGGEOTIFFMAPPROJECTION_H
#define RGGEOTIFFMAPPROJECTION_H

#include "RGMapProjection.h"
#include <geotiff.h>
#include <tiffio.h>

class RGGeoTiffMapProjection : public RGMapProjection
{
    Q_OBJECT
public:
    RGGeoTiffMapProjection(const QString& fileName, QObject* parent = nullptr);
    ~RGGeoTiffMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool fillProjectionTransformation(double& xOrigin, double& yOrigin, double& pixelSize, int& csType) const override
    {
        //Not supported
        return false;
    }

    bool saveProjection(const QString& fileName);

    /**
     * @brief saveProjectionDataToGeoTiff save RGMapProjection in GeoTiff format
     * @param fileName name of the TIFF file
     * @param rgMapProj map projection
     * @return
     */
    static bool saveProjectionDataToGeoTiff(const QString& fileName, const RGMapProjection& rgMapProj);

private:
    void cleanup();

    QString mFileName;
    TIFF* mTiff;
    GTIF* mGTif;

    short mModelType;          // ModelTypeGeographic / ModelTypeProjected / etc.
};

#endif // RGGEOTIFFMAPPROJECTION_H
