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

    RGMapBounds(const QGeoCoordinate& ne, const QGeoCoordinate& sw, int zoom)
        : m_neCoord(ne),
          m_swCoord(sw),
          m_zoom(zoom)
    {
    }

    bool isValid() const
    {
        return m_neCoord.isValid() && m_swCoord.isValid() && m_zoom > 0;
    }

    void setSwCoord(const QGeoCoordinate& swCoord)
    {
        m_swCoord = swCoord;
    }

    void setNeCoord(const QGeoCoordinate& neCoord)
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

    bool fromQVariant(const QVariant& v)
    {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        if (QMetaType::Type(v.typeId()) == QMetaType::QVariantMap)
#else
        if (QMetaType::Type(v.type()) == QMetaType::QVariantMap)
#endif
        {
            QMap<QString, QVariant> bounds = v.toMap();
            setNeCoord(QGeoCoordinate(bounds[QStringLiteral("neLat")].toDouble(), bounds[QStringLiteral("neLng")].toDouble()));
            setSwCoord(QGeoCoordinate(bounds[QStringLiteral("swLat")].toDouble(), bounds[QStringLiteral("swLng")].toDouble()));
            setZoom(bounds[QStringLiteral("zoom")].toInt());
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
        bounds[QStringLiteral("neLat")] = m_neCoord.latitude();
        bounds[QStringLiteral("neLng")] = m_neCoord.longitude();
        bounds[QStringLiteral("swLat")] = m_swCoord.latitude();
        bounds[QStringLiteral("swLng")] = m_swCoord.longitude();
        bounds[QStringLiteral("zoom")] = m_zoom;
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
