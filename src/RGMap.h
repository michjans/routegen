#ifndef RGMAP_H
#define RGMAP_H

#include "RGMapBounds.h"
#include "RGMapProjection.h"

#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QList>
#include <QPoint>

class RGMap : public QObject
{
    Q_OBJECT
public:
    explicit RGMap(QObject *parent = 0);

    bool loadMap(const QString &fileName, const QPixmap &map = QPixmap(),
                 const RGMapBounds &gmapBounds=RGMapBounds());
    bool hasGeoBounds() const;
    QList<QPoint> mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const;
    QString fileName() const;
    bool isEmpty() const;
    bool isDirty() const;
    void resetDirty();
    void clearMap();
    void saveGeoBounds(const QString &fileName);

    void read (const QJsonObject &json);
    void write (QJsonObject &json);

signals:
    void mapLoaded(const QPixmap &map);

private:
    QPixmap mMap;
    QString mFileName;
    bool mDirty;
    std::unique_ptr<RGMapProjection> mMapProjection;
};

#endif // RGMAP_H
