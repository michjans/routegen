#ifndef RGGOOGLEMAPPROJECTION_H
#define RGGOOGLEMAPPROJECTION_H

#include "RGMapProjection.h"

#include "RGGoogleMapBounds.h"

#include <QGeoCoordinate>
#include <QPointF>

class RGGoogleMapProjection : public RGMapProjection
{
    Q_OBJECT
public:
    RGGoogleMapProjection(const RGGoogleMapBounds& mapBounds, QObject* parent = nullptr);
    virtual ~RGGoogleMapProjection() override;

    bool isValid() const override;
    QPoint convert(const QGeoCoordinate& geoPoint) const override;
    bool saveProjection(const QString& fileName) override;

private:
    //Coordinate to world coordinates
    QPointF project(const QGeoCoordinate& geoPoint) const;

    //World to pixels
    QPoint worldToPixel(const QPointF& worldPoint) const;

    RGGoogleMapBounds m_bounds;
    QPoint mTopLeft;
    QPoint mBottomRight;
    int mAntiMeredianPosX;
};

#endif // RGGOOGLEMAPPROJECTION_H
