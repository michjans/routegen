#ifndef RGMAPBOUNDS_H
#define RGMAPBOUNDS_H

#include <QGeoCoordinate>
#include <QVariant>

class RGMapBounds
{
public:
    RGMapBounds()
        : m_zoom(0)
    {
    }

    RGMapBounds(int zoom)
        : m_zoom(zoom)
    {
    }

    virtual ~RGMapBounds() = default;

    //RGMapBounds(const RGMapBounds& rhs) = default;
    //RGMapBounds(RGMapBounds&& rhs) = default;

    void setZoom(int zoom)
    {
        m_zoom = zoom;
    }

    int getZoom() const
    {
        return m_zoom;
    }

    virtual bool isValid() const = 0;
    virtual bool fromQVariant(const QVariant& v) = 0;
    virtual QVariant toQVariant() const = 0;

protected:
    int m_zoom;
};

#endif // RGMAPBOUNDS_H
