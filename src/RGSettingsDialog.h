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

#ifndef RGSETTINGS_DIALOG_H
#define RGSETTINGS_DIALOG_H

#include "RGEncVideo.h"
#include "RGOSMTileProviderManager.h"
#include "ui_settings.h"
#include <QDialog>

class QSpinBox;
class QPushButton;
class QCheckBox;

class RGSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    RGSettingsDialog(RGEncVideo* videoSettings, QWidget* parent = 0);
    ~RGSettingsDialog();
    int getSmoothCoef();
    bool getIconlessBeginEndFrames();

private slots:
    void on_encoderSelectionCB_activated(const QString& text);
    void on_mResetDefaultsPB_clicked(bool);
    void addTileProviderClicked(bool);
    void editTileProviderClicked(bool);
    void removeTileProviderClicked(bool);
    void accept();

private:
    bool validateOSMTileProvider(const RGTileProviderManager::TileProvider &tileProvider);

    QCheckBox* mGenerateBeginEndFramesCB;
    QSpinBox* mSmoothLengthSB;
    QPushButton* mResetDefaultsPB;
    RGEncVideo* mVideoEncoder;
    RGTileProviderManager mTileProviderManager;
    Ui::Dialog ui;
};

#endif
