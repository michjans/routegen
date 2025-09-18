/*
    Copyright (C) 2010-2025  Michiel Jansen

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

#include "RGOsMap.h"
#include "RGSettings.h"

#include <QtWidgets>

#include <QGeoCoordinate>
#include <QGeoRectangle>
#include <QTimer>

RGOsMap::RGOsMap(QWidget* parent, const QGeoPath& geoPath)
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::Dialog | Qt::WindowMaximizeButtonHint)
{
    ui.setupUi(this);

    //ui.progressBar->hide();
    //ui.progressBar->setTextVisible(true);
    //ui.progressBar->setFormat(tr("Number of retrieved tiles: %p"));
    ui.widthScaleSB->setValue(RGSettings::getGMXFactor());
    ui.heightScaleSB->setValue(RGSettings::getGMYFactor());

    //We connect both scale spinboxes to the same slot! So we can't use
    //the passed value, because we don't know the source.
    QObject::connect(ui.widthScaleSB, &QDoubleSpinBox::valueChanged, this, &RGOsMap::handleScaleSpinboxChanged);
    QObject::connect(ui.heightScaleSB, &QDoubleSpinBox::valueChanged, this, &RGOsMap::handleScaleSpinboxChanged);

    QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &RGOsMap::accept);

    RGTileProviderManager tileProviderMgr;
    auto tileProviders = tileProviderMgr.getAllProviders();
    std::for_each(tileProviders.begin(), tileProviders.end(),
                  [this](const auto& tileProvider)
                  {
                      mTileProviders[tileProvider.name] = tileProvider;
                      ui.mapTypeBox->addItem(tileProvider.name);
                  });

    QString activeOsmProvider = RGSettings::getActiveOsmProvider();
    ui.mapTypeBox->setCurrentText(activeOsmProvider);
    ui.osmView->osmBackend().setTileProvider(mTileProviders[activeOsmProvider]);

    //Make sure the zoom spinbox is synced with the view's zoom level and lat/lon coordinates
    QObject::connect(ui.osmView, &RGOsmGraphicsView::zoomLevelChanged, this, &RGOsMap::on_zoomLevelChangedChanged);
    QObject::connect(ui.osmView, &RGOsmGraphicsView::centerCoordChanged, this, &RGOsMap::on_centerCoordChanged);

    //Connect progress monitor
    QObject::connect(ui.osmView, &RGOsmGraphicsView::initTileProgress, this, &RGOsMap::handleInitTileProgress);
    QObject::connect(ui.osmView, &RGOsmGraphicsView::tileProgress, this, &RGOsMap::handleTileProgress);
    QObject::connect(ui.osmView, &RGOsmGraphicsView::allTilesReceived, this, &RGOsMap::handleAllTilesReceived);

    //Init map resolution
    handleScaleSpinboxChanged(1.0);

    if (geoPath.size() > 1)
    {
        ui.osmView->setGeoPath(geoPath);

        //Immediately load the map in case a route is passed
        on_goButton_clicked(true);
    }

    //Shared setting for google/osm map import dialog
    setGeometry(RGSettings::getGoogleMapDialogGeometry());
}

void RGOsMap::accept()
{
    m_map = ui.osmView->renderMap();

    RGSettings::setGMXFactor(ui.widthScaleSB->value());
    RGSettings::setGMYFactor(ui.heightScaleSB->value());
    RGSettings::setGoogleMapDialogGeometry(geometry());

    QDialog::accept();
}

void RGOsMap::on_goButton_clicked(bool)
{
    QGeoCoordinate coord(ui.latSB->value(), ui.lonSB->value());
    m_mapBounds.setZoom(ui.zoomBox->value());
    m_mapBounds.setCenterCoord(coord);
    auto res = RGSettings::getOutputResolution();
    ui.osmView->loadMap(coord, ui.zoomBox->value(), QSize(ui.widthScaleSB->value() * res.width(), ui.heightScaleSB->value() * res.height()));
}

void RGOsMap::handleScaleSpinboxChanged(double)
{
    //multiply scale factors by current output resolution width/height
    auto res = RGSettings::getOutputResolution();
    ui.osmView->setFixedSceneResolution(QSize(ui.widthScaleSB->value() * res.width(), ui.heightScaleSB->value() * res.height()));
}

void RGOsMap::on_mapTypeBox_textActivated(const QString& text)
{
    qDebug() << "on_mapTypeBox_textActivated:" << text;
    RGSettings::setActiveOsmProvider(text);
    ui.osmView->osmBackend().setTileProvider(mTileProviders[text]);
}

void RGOsMap::on_zoomLevelChangedChanged(int zoom)
{
    ui.zoomBox->setValue(zoom);
    m_mapBounds.setZoom(zoom);
}

void RGOsMap::on_centerCoordChanged(const QGeoCoordinate& centerCoord)
{
    ui.latSB->setValue(centerCoord.latitude());
    ui.lonSB->setValue(centerCoord.longitude());
    m_mapBounds.setCenterCoord(centerCoord);
}

void RGOsMap::handleInitTileProgress(int totalTiles)
{
    //Tile loading has started, block other requests
    ui.goButton->setDisabled(true);
    ui.progressBar->setRange(0, totalTiles);
}

void RGOsMap::handleTileProgress(int progress)
{
    ui.progressBar->setValue(progress);
}

void RGOsMap::handleAllTilesReceived()
{
    ui.goButton->setDisabled(false);
    ui.progressBar->reset();
}
