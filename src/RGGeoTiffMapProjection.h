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
    bool saveProjection(const QString& fileName);

private:
    QString mFileName;
    TIFF* mTiff;
    GTIF* mGTif;
};

#endif // RGGEOTIFFMAPPROJECTION_H
