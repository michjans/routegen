#ifndef RGGEOTIFFMAPPROJECTION_H
#define RGGEOTIFFMAPPROJECTION_H

#include "RGMapProjection.h"
#include "geotiff.h"
#include "tiffio.h"

class RGGeoTiffMapProjection : public RGMapProjection
{
public:
    RGGeoTiffMapProjection(const QString &fileName);
    ~RGGeoTiffMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    void saveProjection(const QString &fileName) override;

private:
    TIFF *mTiff;
    GTIF *mGTif;
};

#endif // RGGEOTIFFMAPPROJECTION_H
