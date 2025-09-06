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
protected:
    //RGMapProjection();
};

#endif // RGMAPPROJECTION_H
