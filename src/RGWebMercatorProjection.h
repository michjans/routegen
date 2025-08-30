#ifndef RGWEBMERCATORPROJECTION_H
#define RGWEBMERCATORPROJECTION_H

#include "RGMapProjection.h"

#include "RGGoogleMapBounds.h"
#include "RGOsMapBounds.h"

#include <QGeoCoordinate>
#include <QPointF>

class RGWebMercatorProjection : public RGMapProjection
{
    Q_OBJECT
public:
    RGWebMercatorProjection(const RGGoogleMapBounds& mapBounds, QObject* parent = nullptr);
    RGWebMercatorProjection(const RGOsMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent = nullptr);
    virtual ~RGWebMercatorProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    QGeoCoordinate pixelToGeo(const QPoint& pixel) const;
    bool saveProjection(const QString& fileName) override;

private:
    //Coordinate to world coordinates
    QPointF project(const QGeoCoordinate& geoPoint) const;
    QGeoCoordinate unproject(const QPointF& worldPoint) const;

    //World to pixels
    QPoint worldToPixel(const QPointF& worldPoint) const;
    QPointF pixelToWorld(const QPoint& pixel) const;

    RGGoogleMapBounds m_bounds;
    QPoint mTopLeft;
    QPoint mBottomRight;
    int mAntiMeredianPosX;
};

#endif // RGWEBMERCATORPROJECTION_H
