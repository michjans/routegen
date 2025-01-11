/*
    Copyright (C) 2010-2011  Michiel Jansen

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
    : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::Dialog | Qt::WindowMaximizeButtonHint),
      m_geoPath(geoPath)
{
    ui.setupUi(this);

    ui.progressBar->hide();
    ui.progressBar->setRange(0, 100);
    ui.widthScaleSB->setValue(RGSettings::getGMXFactor());
    ui.heightScaleSB->setValue(RGSettings::getGMYFactor());

    //We connect both scale spinboxes to the same slot! So we can't use
    //the passed value, because we don't know the source.
    QObject::connect(ui.widthScaleSB, &QDoubleSpinBox::valueChanged, this, &RGOsMap::handleScaleSpinboxChanged);
    QObject::connect(ui.heightScaleSB, &QDoubleSpinBox::valueChanged, this, &RGOsMap::handleScaleSpinboxChanged);

    QObject::connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &RGOsMap::on_accept);

    //TODO: Different OSMap providers
    ui.mapTypeBox->insertItem(0, QStringLiteral("osm"));
    ui.mapTypeBox->setCurrentIndex(0);

    //Init map resolution
    handleScaleSpinboxChanged(1.0);

    if (geoPath.size() > 1)
    {
        QGeoRectangle startGeoRect = geoPath.boundingGeoRectangle();
        //Load initial map location from incoming geo rectangle
        qDebug() << "startGeoRect:"
                 << "  topLeft:" << startGeoRect.topLeft() << "  bottomLeft:" << startGeoRect.bottomLeft() << "  topRight:" << startGeoRect.topRight()
                 << "  bottomRight:" << startGeoRect.bottomRight() << "  center:" << startGeoRect.center();
        //TODO: Draw rectangle above osm map

        ui.latSB->setEnabled(false);
        ui.lonSB->setEnabled(false);
        ui.goButton->setEnabled(false);
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

void RGOsMap::on_accept()
{
    if (m_geoPath.size() > 1)
    {
        //TODO: Do we need this extra stage?
        //We need to wait until the georect is deleted
        QTimer::singleShot(1000, this, &RGOsMap::continue_Accept);

        setCursor(Qt::WaitCursor);
    }
    else
    {
        continue_Accept();
    }
}

void RGOsMap::continue_Accept()
{
    //Now retrieve the map boundaries from google's map
    this->accept();
}

void RGOsMap::on_goButton_clicked(bool)
{
    //TODO: Monitor progress
    // ui.progressBar->hide();
    // ui.progressBar->setValue(progress);
    // ui.progressBar->show();

    QGeoCoordinate coord(ui.latSB->value(), ui.lonSB->value());
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
}

void RGOsMap::on_zoomBox_valueChanged(int zoom)
{
    //TODO?
}
