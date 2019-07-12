#ifndef RGMAP_H
#define RGMAP_H

#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QList>
#include <QGeoCoordinate>
#include <QPoint>

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
