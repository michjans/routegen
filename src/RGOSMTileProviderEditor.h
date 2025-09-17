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

#ifndef RBOSMPROVIDEREDITOR_H
#define RBOSMPROVIDEREDITOR_H

#include <QDialog>

#include "RGOSMTileProviderManager.h"

namespace Ui
{
class OsmProviderEditor;
}

class RGOSMTileProviderEditor : public QDialog
{
    Q_OBJECT

public:
    explicit RGOSMTileProviderEditor(QWidget* parent = nullptr);
    explicit RGOSMTileProviderEditor(const RGTileProviderManager::TileProvider& tileProvider, QWidget* parent = nullptr);
    ~RGOSMTileProviderEditor();

    RGTileProviderManager::TileProvider tileProvider();

private:
    Ui::OsmProviderEditor* ui;
};

#endif // RBOSMPROVIDEREDITOR_H
