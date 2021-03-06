#ifndef RGMAPBOUNDS_H
#define RGMAPBOUNDS_H

#include <QGeoCoordinate>
#include <QMap>
#include <QVariant>

class RGMapBounds
{
public:
    RGMapBounds()
        : m_neCoord(),
          m_swCoord(),
          m_zoom(0)
    {
    }

    RGMapBounds(const QGeoCoordinate &ne, const QGeoCoordinate &sw, int zoom)
        : m_neCoord(ne),
          m_swCoord(sw),
          m_zoom(zoom)
    {
    }

    bool isValid() const
    {
        return m_neCoord.isValid() && m_swCoord.isValid() && m_zoom > 0;
    }

    void setSwCoord(const QGeoCoordinate &swCoord)
    {
        m_swCoord = swCoord;
    }

    void setNeCoord(const QGeoCoordinate &neCoord)
    {
        m_neCoord = neCoord;
    }

    void setZoom(int zoom)
    {
        m_zoom = zoom;
    }

    const QGeoCoordinate& getNE() const
    {
        return m_neCoord;
    }

    const QGeoCoordinate& getSW() const
    {
        return m_swCoord;
    }

    int getZoom() const
    {
        return m_zoom;
    }

    bool fromQVariant(const QVariant &v)
    {
        if (QMetaType::Type(v.type()) == QMetaType::QVariantMap)
        {
            QMap<QString, QVariant> bounds = v.toMap();
            setNeCoord(QGeoCoordinate(bounds["neLat"].toDouble(), bounds["neLng"].toDouble()));
            setSwCoord(QGeoCoordinate(bounds["swLat"].toDouble(), bounds["swLng"].toDouble()));
            setZoom(bounds["zoom"].toInt());
            return isValid();
        }
        else
        {
            return false;
        }
    }

    QVariant toQVariant() const
    {
        QMap<QString, QVariant> bounds;
        bounds["neLat"] = m_neCoord.latitude();
        bounds["neLng"] = m_neCoord.longitude();
        bounds["swLat"] = m_swCoord.latitude();
        bounds["swLng"] = m_swCoord.longitude();
        bounds["zoom"] = m_zoom;
        return QVariant(bounds);
    }

private:
    //Used for Google maps projection method based on Google Maps API documentation
    //https://developers.google.com/maps/documentation/javascript/examples/map-coordinates
    QGeoCoordinate m_neCoord;
    QGeoCoordinate m_swCoord;
    int m_zoom;
};

#endif // RGMAPBOUNDS_H
