#ifndef RGMAPPROJECTION_H
#define RGMAPPROJECTION_H

#include <QGeoCoordinate>
#include <QPoint>
#include <QString>

class RGMapProjection
{
public:
    virtual ~RGMapProjection()
    {
    }

    virtual bool isValid() const = 0;
    virtual QPoint convert(const QGeoCoordinate& geoPoint) const = 0;
    virtual bool saveProjection(const QString& fileName) = 0;

protected:
    //RGMapProjection();
};

#endif // RGMAPPROJECTION_H
