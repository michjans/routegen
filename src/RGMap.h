#ifndef RGMAP_H
#define RGMAP_H

#include "RGMapProjection.h"

#include <QList>
#include <QObject>
#include <QPixmap>
#include <QPoint>
#include <QRectF>

class RGMap : public QObject
{
    Q_OBJECT
public:
    explicit RGMap(QObject* parent = 0);

    /**
     * @brief loadMap loads the map passed by fileName or uses passed map. If the map was saved with geo reference data
     *                the related map projection will be loaded as well.
     * @param fileName the full path to the map file to load
     * @param map a valid QImage is already passed, only the related geo reference data will be loaded (if available)
     * @return true if successfull
     */
    bool loadMap(const QString& fileName, QImage map = QImage());

    /**
     * @brief loadImportedMap loads the imported map and save the geo reference data
     * @param fileName name of the file
     * @param map the map image
     * @param mapBounds geographic projection information
     * @return true if successfull
     */
    template <typename MAPBOUNDS> bool loadImportedMap(const QString& fileName, const QPixmap& map, const MAPBOUNDS& mapBounds);

    bool hasGeoBounds() const;
    QList<QPoint> mapRoutePoints(const QList<QGeoCoordinate>& geoCoordinates) const;
    QString fileName() const;
    bool isEmpty() const;
    bool isDirty() const;
    void resetDirty();
    void clearMap();

    /**
     * @brief Tries to save the geoBounds to a newFile, this will not succeed if the specified
     *        file format does not support geographic boundaries.
     *        Norally only TIFF files can support Geo information or the existing map was imported
     *        using Google maps. In that case the geograpic information is stored outside the map.s
     * @param fileName
     */
    bool saveGeoBoundsToNewFile(const QString& fileName);

    void read(const QJsonObject& json);
    void write(QJsonObject& json);

signals:
    void mapLoaded(const QPixmap& map);

private:
    bool saveProjection(const QString& fileName);

    QPixmap mMap;
    QString mFileName;
    bool mDirty;
    std::unique_ptr<RGMapProjection> mMapProjection;
};

#endif // RGMAP_H
