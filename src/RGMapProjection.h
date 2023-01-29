#ifndef RGMAPPROJECTION_H
#define RGMAPPROJECTION_H

#include <QGeoCoordinate>
#include <QPoint>

class RGMapProjection
{
public:
    virtual ~RGMapProjection() {}

    virtual bool isValid() const = 0;
    virtual QPoint convert(const QGeoCoordinate& geoPoint) const = 0;

protected:
    //RGMapProjection();
};

#endif // RGMAPPROJECTION_H
