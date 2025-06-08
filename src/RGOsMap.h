/*
    Copyright (C) 2008-2011  Michiel Jansen

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

#include "RGOSMTileProviderManager.h"
#include "RGOsMapBounds.h"

#include "ui_osmap.h"

#include <QGeoPath>
#include <QPixmap>
#include <QString>

class RGOsMap : public QDialog
{
    Q_OBJECT

public:
    RGOsMap(QWidget* parent, const QGeoPath& geoPath = QGeoPath());

    QPixmap getMap() const
    {
        return m_map;
    }
    const RGOsMapBounds& getMapBounds() const
    {
        return m_mapBounds;
    }

public slots:
    void accept() override;

private slots:
    void on_accept();
    void continue_Accept();
    void on_goButton_clicked(bool);
    void handleScaleSpinboxChanged(double);
    void on_mapTypeBox_textActivated(const QString&);
    void on_zoomLevelChangedChanged(int zoom);
    void on_centerCoordChanged(const QGeoCoordinate& centerCoord);

private:
    Ui_osMap ui;
    QPixmap m_map;
    QGeoPath m_geoPath;
    RGOsMapBounds m_mapBounds;
    std::map<QString, RGTileProviderManager::TileProvider> mTileProviders;
};
