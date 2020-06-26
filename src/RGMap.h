#ifndef RGMAP_H
#define RGMAP_H

#include "RGMapBounds.h"

#include <QObject>
#include <QPixmap>
#include <QRectF>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QGeoCoordinate>

class RGMap : public QObject
{
    Q_OBJECT
public:
    explicit RGMap(QObject *parent = 0);

    bool loadMap(const QString &fileName, const QPixmap &map = QPixmap(),
                 const RGMapBounds &bounds=RGMapBounds());
    bool hasGeoBounds() const;
    RGMapBounds geoBounds() const;
    QList<QPoint> mapRoutePoints(const QList<QGeoCoordinate> &geoCoordinates) const;
    QString fileName() const;
    bool isEmpty() const;
    bool isDirty() const;
    void resetDirty();
    void clearMap();

    void read (const QJsonObject &json);
    void write (QJsonObject &json);

signals:
    void mapLoaded(const QPixmap &map);

public slots:

private:
    //Coordinate to world coordinates
    QPointF project(const QGeoCoordinate& geoPoint) const;

    //World to pixels
    QPoint worldToPixel(const QPointF &worldPoint) const;


    QPixmap mMap;
    QString mFileName;
    bool mDirty;
    RGMapBounds m_bounds;

    QPoint mTopLeft;
    QPoint mBottomRight;
    int mAntiMeredianPosX;
};

#endif // RGMAP_H
