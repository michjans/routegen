#ifndef RGMAPPROJECTION_H
#define RGMAPPROJECTION_H

#include <QGeoCoordinate>
#include <QObject>
#include <QPoint>
#include <QString>

class RGMapProjection : public QObject
{
    Q_OBJECT
public:
    RGMapProjection(QObject* parent = nullptr);
    RGMapProjection(const RGMapProjection&) = delete;
    RGMapProjection(RGMapProjection&&) = delete;
    RGMapProjection& operator=(const RGMapProjection&) = delete;
    RGMapProjection& operator=(RGMapProjection&&) = delete;
    ~RGMapProjection() override
    {
    }

    virtual bool isValid() const = 0;
    virtual QPoint convert(const QGeoCoordinate& geoPoint) const = 0;

    /**
     * @brief fillProjectionTransformation fill projection transformation data, which can be used to save this
     *        projection information in another format (e.g. GeoTiff...what a coincidence!)
     * @param xOrigin top-left X in projection units
     * @param yOrigin top-left Y in projection units
     * @param pixelSize height of a pixel (negative, since Y decreases downward)
     * @param csType projection coding, e.g. 3857 for EPSG:3857
     * @return
     */
    virtual bool fillProjectionTransformation(double& xOrigin, double& yOrigin, double& pixelSize, int& csType) const = 0;

protected:
    //RGMapProjection();
};

#endif // RGMAPPROJECTION_H
