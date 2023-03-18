#ifndef RGGOOGLEMAPPROJECTION_H
#define RGGOOGLEMAPPROJECTION_H

#include "RGMapProjection.h"

#include "RGMapBounds.h"

#include <QGeoCoordinate>
#include <QPointF>

class RGGoogleMapProjection : public RGMapProjection
{
public:
    RGGoogleMapProjection(const RGMapBounds& mapBounds);
    virtual ~RGGoogleMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool saveProjection(const QString& fileName) override;

private:
    //Coordinate to world coordinates
    QPointF project(const QGeoCoordinate& geoPoint) const;

    //World to pixels
    QPoint worldToPixel(const QPointF& worldPoint) const;

    RGMapBounds m_bounds;
    QPoint mTopLeft;
    QPoint mBottomRight;
    int mAntiMeredianPosX;
};

#endif // RGGOOGLEMAPPROJECTION_H
