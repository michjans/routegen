#ifndef RGGEOTIFFMAPPROJECTION_H
#define RGGEOTIFFMAPPROJECTION_H

#include "RGMapProjection.h"
#include "geotiff.h"
#include "tiffio.h"

class RGGeoTiffMapProjection : public RGMapProjection
{
public:
    RGGeoTiffMapProjection(const QString& fileName);
    ~RGGeoTiffMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool saveProjection(const QString& fileName) override;

private:
    QString mFileName;
    TIFF* mTiff;
    GTIF* mGTif;
};

#endif // RGGEOTIFFMAPPROJECTION_H
