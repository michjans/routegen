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

#include "RGOSMTileProviderEditor.h"
#include "ui_osmtileprovidereditor.h"

RGOSMTileProviderEditor::RGOSMTileProviderEditor(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::OsmProviderEditor)
{
    ui->setupUi(this);
    ui->tileProviderEdit->setFocus();
}

RGOSMTileProviderEditor::RGOSMTileProviderEditor(const RGTileProviderManager::TileProvider& tileProvider, QWidget* parent)
    : RGOSMTileProviderEditor(parent)
{
    ui->tileProviderEdit->setEnabled(false);
    ui->tileProviderEdit->setText(tileProvider.name);
    ui->tileProviderURL->setText(tileProvider.urlTemplate);
    ui->tileProviderAttribution->setText(tileProvider.attribution);
    ui->tileProviderURL->setFocus();
}

RGOSMTileProviderEditor::~RGOSMTileProviderEditor()
{
    delete ui;
}

RGTileProviderManager::TileProvider RGOSMTileProviderEditor::tileProvider()
{
    RGTileProviderManager::TileProvider tileProvider;
    tileProvider.name = ui->tileProviderEdit->text();
    tileProvider.urlTemplate = ui->tileProviderURL->text();
    tileProvider.attribution = ui->tileProviderAttribution->text();

    return tileProvider;
}
