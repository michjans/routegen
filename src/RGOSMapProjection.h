#ifndef RGOSMapProjection_H
#define RGOSMapProjection_H

#include "RGMapProjection.h"
#include "RGOsMapBounds.h"

#include <QGeoCoordinate>
#include <QPointF>

class RGOSMapProjection : public RGMapProjection
{
    Q_OBJECT
public:
    static const int TILE_SIZE = 256;

    RGOSMapProjection(const RGOsMapBounds& osmBoumds, int mapWidth, int mapHeight, QObject* parent = nullptr);
    ~RGOSMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool saveProjection(const QString& fileName) override;
    static QPointF latLonToTile(const QGeoCoordinate& geoPoint, int zoom);
    static QGeoCoordinate tileToLatLon(const QPointF& tileXY, int zoom);

private:
    RGOsMapBounds m_osmBoumds;
    int m_mapWidth;
    int m_mapHeight;
};

#endif // RGOSMapProjection_H
