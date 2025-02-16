#ifndef RGOSMMAPBOUNDS_H
#define RGOSMMAPBOUNDS_H

#include "RGMapBounds.h"

#include <QMap>

class RGOsMapBounds : public RGMapBounds
{
public:
    RGOsMapBounds()
        : RGMapBounds()
    {
    }

    RGOsMapBounds(const QGeoCoordinate& centerPoint, int zoom)
        : RGMapBounds(zoom),
          m_centerPoint(centerPoint)
    {
    }

    ~RGOsMapBounds() override = default;

    bool isValid() const override
    {
        return m_centerPoint.isValid() && m_zoom > 0;
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
            setCenterCoord(QGeoCoordinate(bounds[QStringLiteral("centerCoordLat")].toDouble(), bounds[QStringLiteral("centerCoordLng")].toDouble()));
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
        bounds[QStringLiteral("centerCoordLat")] = m_centerPoint.latitude();
        bounds[QStringLiteral("centerCoordLng")] = m_centerPoint.longitude();
        bounds[QStringLiteral("zoom")] = m_zoom;
        return QVariant(bounds);
    }

    void setCenterCoord(const QGeoCoordinate& centerCoord)
    {
        m_centerPoint = centerCoord;
    }

    const QGeoCoordinate& getCenterCoord() const
    {
        return m_centerPoint;
    }

private:
    QGeoCoordinate m_centerPoint;
};

#endif // RGOSMMAPBOUNDS_H
