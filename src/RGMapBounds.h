#ifndef RGMAPBOUNDS_H
#define RGMAPBOUNDS_H

#include <QGeoCoordinate>

class RGMapBounds
{
public:
    RGMapBounds()
        : m_neCoord(),
          m_swCoord(),
          m_zoom(-1)
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
        return m_neCoord.isValid() && m_swCoord.isValid() && m_zoom > -1;
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

private:
    //Used for Google maps projection method based on Google Maps API documentation
    //https://developers.google.com/maps/documentation/javascript/examples/map-coordinates
    QGeoCoordinate m_neCoord;
    QGeoCoordinate m_swCoord;
    int m_zoom;
};

#endif // RGMAPBOUNDS_H
