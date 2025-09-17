/*
    Copyright (C) 2008-2025  Michiel Jansen

This file is part of Route Generator.

Route Generator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


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
