#ifndef RGGOOGLEMAPBOUNDS_H
#define RGGOOGLEMAPBOUNDS_H

#include "RGMapBounds.h"

#include <QMap>

class RGGoogleMapBounds : public RGMapBounds
{
public:
    RGGoogleMapBounds()
        : RGMapBounds()
    {
    }

    RGGoogleMapBounds(const QGeoCoordinate& ne, const QGeoCoordinate& sw, int zoom)
        : RGMapBounds(zoom),
          m_neCoord(ne),
          m_swCoord(sw)
    {
    }

    ~RGGoogleMapBounds() override = default;

    bool isValid() const override
    {
        return m_neCoord.isValid() && m_swCoord.isValid() && m_zoom > 0;
    }
    
    bool fromQVariant(const QVariant& v) override
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

    QVariant toQVariant() const override
    {
        QMap<QString, QVariant> bounds;
        bounds[QStringLiteral("neLat")] = m_neCoord.latitude();
        bounds[QStringLiteral("neLng")] = m_neCoord.longitude();
        bounds[QStringLiteral("swLat")] = m_swCoord.latitude();
        bounds[QStringLiteral("swLng")] = m_swCoord.longitude();
        bounds[QStringLiteral("zoom")] = m_zoom;
        return QVariant(bounds);
    }

    void setSwCoord(const QGeoCoordinate& swCoord)
    {
        m_swCoord = swCoord;
    }

    void setNeCoord(const QGeoCoordinate& neCoord)
    {
        m_neCoord = neCoord;
    }

    const QGeoCoordinate& getNE() const
    {
        return m_neCoord;
    }

    const QGeoCoordinate& getSW() const
    {
        return m_swCoord;
    }

private:
    //Used for Google maps projection method based on Google Maps API documentation
    //https://developers.google.com/maps/documentation/javascript/examples/map-coordinates
    QGeoCoordinate m_neCoord;
    QGeoCoordinate m_swCoord;
};

#endif // RGGOOGLEMAPBOUNDS_H
