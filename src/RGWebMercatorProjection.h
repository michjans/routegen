#ifndef RGWEBMERCATORPROJECTION_H
#define RGWEBMERCATORPROJECTION_H

#include "RGMapProjection.h"

#include "RGGoogleMapBounds.h"
#include "RGOsMapBounds.h"

#include <QGeoCoordinate>
#include <QImage>
#include <QPointF>

class RGWebMercatorProjection : public RGMapProjection
{
    Q_OBJECT
public:
    /**
     * @brief RGWebMercatorProjection instantiotes the RGWebMercatorProjection from a RGGoogleMapBounds
     * @param mapBounds the RGGoogleMapBounds
     * @param mapWidth width of map
     * @param mapHeight height of map
     * @param parent
     */
    RGWebMercatorProjection(const RGGoogleMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent = nullptr);

    /**
     * @brief RGWebMercatorProjection instantiotes the RGWebMercatorProjection from a RGOsMapBounds
     * @param mapBounds the RGOsMapBounds
     * @param mapWidth width of map
     * @param mapHeight height of map
     * @param parent
     */
    RGWebMercatorProjection(const RGOsMapBounds& mapBounds, int mapWidth, int mapHeight, QObject* parent = nullptr);

    /**
     * @brief RGWebMercatorProjection instantiotes the RGWebMercatorProjection from an image stored on disk,
     *        assuming that the required georefernce projection keys are stored within the file itself
     *        If the QImage will not contain the georeference keys, the projection will be marked as invalid
     * @param map the QImage containing the geo reference keys
     * @param parent
     */
    RGWebMercatorProjection(const QImage& map, QObject* parent = nullptr);
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

    int m_zoom;
    QPoint mTopLeft;
    QPoint mBottomRight;
    int mAntiMeredianPosX;
};

#endif // RGWEBMERCATORPROJECTION_H
