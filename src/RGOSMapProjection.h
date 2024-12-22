#ifndef RGOSMapProjection_H
#define RGOSMapProjection_H

#include "RGMapProjection.h"

#include <QGeoCoordinate>
#include <QPointF>

class RGOSMapProjection : public RGMapProjection
{
    Q_OBJECT
public:
    static const int TILE_SIZE = 256;

    RGOSMapProjection(const QGeoCoordinate& centerPoint, int zoom, int mapWidth, int mapHeight, QObject* parent = nullptr);
    ~RGOSMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool saveProjection(const QString& fileName) override;
    static QPointF latLonToTile(const QGeoCoordinate& geoPoint, int zoom);
    static QGeoCoordinate tileToLatLon(const QPointF& tileXY, int zoom);

private:

    QGeoCoordinate m_centerPoint;
    int m_zoom;
    int m_mapWidth;
    int m_mapHeight;
};

#endif // RGOSMapProjection_H
