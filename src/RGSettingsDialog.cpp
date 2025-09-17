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
    QObject::connect(ui.editOsmProviderPB, &QPushButton::clicked, this, &RGSettingsDialog::editTileProviderClicked);
    QObject::connect(ui.removeOsmProviderPB, &QPushButton::clicked, this, &RGSettingsDialog::removeTileProviderClicked);
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
        if (validateOSMTileProvider(tileProvider))
        {
            if (mTileProviderManager.addCustomProvider(tileProvider))
            {
                ui.osmProviderList->addItem(tileProvider.name);
            }
            else
            {
                //Not added
                QMessageBox::warning(this, tr("Provider not added"), tr("Tile provider not added. Name already exists or empty?"));
            }
        }
    }
}

void RGSettingsDialog::editTileProviderClicked(bool)
{
    auto currentItem = ui.osmProviderList->currentItem();
    if (currentItem)
    {
        auto existingTileProvider = mTileProviderManager.getCustomProviderByName(currentItem->text());
        if (!existingTileProvider)
        {
            return;
        }
        RGOSMTileProviderEditor tileProviderDialog(*existingTileProvider, this);
        if (tileProviderDialog.exec() == QDialog::Accepted)
        {
            auto tileProvider = tileProviderDialog.tileProvider();
            if (validateOSMTileProvider(tileProvider))
            {
                mTileProviderManager.replaceCustomProvider(tileProvider);
            }
        }
    }
}

void RGSettingsDialog::removeTileProviderClicked(bool)
{
    auto currentItem = ui.osmProviderList->currentItem();
    if (currentItem)
    {
        mTileProviderManager.removeCustomProvider(currentItem->text());
        //Assueme it's removed correctly, so delete the item as well
        delete currentItem;
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

bool RGSettingsDialog::validateOSMTileProvider(const RGTileProviderManager::TileProvider &tileProvider)
{

#if 0
    //TODO: Somehow always results in false! Check!
    bool validUrl = false;
    QUrl url(tileProvider.urlTemplate);
    if (!url.isValid() || (url.scheme() != "http" && url.scheme() != "https")) {
        validUrl = false;
    } else {
        QString path = url.path();
        if (path.contains("{zoom}") && path.contains("{x}") && path.contains("{y}")) {
            validUrl = true;
        }
    }
#else
    bool validUrl = true;
#endif

    if (!validUrl)
    {
        QMessageBox::warning(this, tr("Incorrect URL format"),
                             tr("<html><head/><body><p>Enter URL to tile provider in format of e.g.:</p><p><span style=\" text-decoration: underline;\">"
                                "https://{s}.tile.openstreetmap.org/{zoom}/{x}/{y}.png</span></p><p>Note that the {s} (subdomain part) is optional and"
                                "will be replaced by a, b, or c if provided.</p>"
                                "<p>For an overview of OSM tile provider see: <a href=\"https://wiki.openstreetmap.org/wiki/Raster_tile_providers\">"
                                "OSM Raster tile providers</span></a></p></body></html>"));
        return false;
    }
    if (tileProvider.attribution.isEmpty())
    {
        QMessageBox::warning(this, tr("Empty attribution text"),
                             tr("<html><head/><body><p>Attribution text may not be empty when the Produced Work is used Publicly.</p>"
                                "<p>See: <a href=\"https://osmfoundation.org/wiki/Licence/Attribution_Guidelines\">OSM Attribution Guidelines</a></p></body></html>"));
    }

    return true;;
}
