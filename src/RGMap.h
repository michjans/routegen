#ifndef RGMAP_H
#define RGMAP_H

#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QList>
#include <QPoint>

//Under linux Ubuntu 16 which doesn't have QGeoCoordinate and QGeoPath
#ifndef UBUNTU_DEBUG
#include <QGeoCoordinate>
#else
class QGeoCoordinate : public QObject
{
    Q_OBJECT
public:
    QGeoCoordinate(float lat, float lon)
        : QObject(),
          mLat(lat),
          mLon(lon)
    {

    }

    QGeoCoordinate(const QGeoCoordinate &src)
        : QObject(),
          mLat(src.mLat),
          mLon(src.mLon)
    {

    }

    QGeoCoordinate& operator=(const QGeoCoordinate &other)
    {
        mLat = other.mLat;
        mLon = other.mLon;
        return *this;
    }

    float longitude() const
    {
        return mLon;
    }
    float latitude() const
    {
        return mLat;
    }


private:
    float mLat, mLon;
};
#endif

class RGMap : public QObject
{
    Q_OBJECT
public:
    explicit RGMap(QObject *parent = 0);

    bool loadMap(const QString &fileName, const QPixmap &map = QPixmap(), const QRectF mapBounds = QRectF());
    bool hasWorldBounds() const;
    QList<QPoint> mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const;
    QString fileName() const;

signals:
    void mapLoaded(const QPixmap &map);

public slots:

private:
    QRectF mWorldBounds;
    QPixmap mMap;
    QString mFileName;
};

#endif // RGMAP_H
