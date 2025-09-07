#ifndef RGGEOTIFFMAPPROJECTION_H
#define RGGEOTIFFMAPPROJECTION_H

#include "RGMapProjection.h"
#include "geotiff.h"
#include "tiffio.h"

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
    QString mFileName;
    TIFF* mTiff;
    GTIF* mGTif;
};

#endif // RGGEOTIFFMAPPROJECTION_H
