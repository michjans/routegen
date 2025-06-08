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

#include <QDebug>
#include <QtGui>

#include "RGOSMTileProviderEditor.h"
#include "RGSettings.h"
#include "RGSettingsDialog.h"

RGSettingsDialog::RGSettingsDialog(RGEncVideo* videoSettings, QWidget* parent)
    : QDialog(parent),
      mVideoEncoder(videoSettings)
{
    ui.setupUi(this);

#ifdef Q_OS_WIN
    ui.encoderSelectionCB->addItem("bmp2avi");
    ui.encoderSelectionCB->addItem("ffmpeg");
#else
    //No bmp2avi on Linux, so nu use to display this option
    ui.encoderSelectionCB->addItem(QStringLiteral("ffmpeg"));
    ui.encoderSelectionCB->setEnabled(false);
#endif

    mGenerateBeginEndFramesCB = ui.mIconLessBeginEndFramesCB;
    mSmoothLengthSB = ui.mSmoothLengthSB;
    mResetDefaultsPB = ui.mResetDefaultsPB;

    //Advanced tab
    mSmoothLengthSB->setValue(RGSettings::getSmoothLength());
    mGenerateBeginEndFramesCB->setChecked(!RGSettings::getIconLessBeginEndFrames());
    ui.encoderSelectionCB->setCurrentIndex(ui.encoderSelectionCB->findText(RGSettings::getVideoEncoder()));

    ui.tabWidget->insertTab(0, videoSettings, tr("Movie Generation"));
    ui.tabWidget->setCurrentIndex(0);

    auto customProviders = mTileProviderManager.getCustomProviders();
    for (const auto& tileProvider : customProviders)
    {
        ui.osmProviderList->addItem(tileProvider.name);
    }

    QObject::connect(ui.addOsmProviderPB, &QPushButton::clicked, this, &RGSettingsDialog::addTileProviderClicked);
}

RGSettingsDialog::~RGSettingsDialog()
{
    ui.tabWidget->widget(0)->setParent(nullptr);
}

int RGSettingsDialog::getSmoothCoef()
{
    return mSmoothLengthSB->value();
}

bool RGSettingsDialog::getIconlessBeginEndFrames()
{
    return !mGenerateBeginEndFramesCB->isChecked();
}

void RGSettingsDialog::on_mResetDefaultsPB_clicked(bool)
{
    mSmoothLengthSB->setValue(RGSettings::getSmoothLength(true));
}

void RGSettingsDialog::addTileProviderClicked(bool)
{
    RGOSMTileProviderEditor tileProviderDialog(this);
    if (tileProviderDialog.exec() == QDialog::Accepted)
    {
        auto tileProvider = tileProviderDialog.tileProvider();
        mTileProviderManager.addCustomProvider(tileProvider);
        ui.osmProviderList->addItem(tileProvider.name); //TODO add signal to mTileProviderManagaer that provider was added
    }
}

void RGSettingsDialog::on_encoderSelectionCB_activated(const QString& /*text*/)
{
}

void RGSettingsDialog::accept()
{
    //Advanced settings
    if (mVideoEncoder->encoderName() != ui.encoderSelectionCB->currentText())
    {
        QMessageBox::StandardButton answer =
            QMessageBox::question(this, tr("Encoder changed"), tr("Changing the encoder will reset the movie generation codec settings. Continue?"),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            //Reset codec selection to default
            RGSettings::setAviCompression(QString());
        }
        else
        {
            ui.encoderSelectionCB->setCurrentIndex(ui.encoderSelectionCB->findText(RGSettings::getVideoEncoder()));
            return;
        }
    }

    RGSettings::setVideoEncoder(ui.encoderSelectionCB->currentText());
    RGSettings::setSmoothLength(mSmoothLengthSB->value());
    RGSettings::setIconLessBeginEndFrames(!mGenerateBeginEndFramesCB->isChecked());
    mTileProviderManager.saveCustomProviders();

    QDialog::accept();
}
