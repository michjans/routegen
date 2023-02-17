/*
    Copyright (C) 2009-2011  Michiel Jansen
    Copyright (C) 2010-2011  Fabien Valthier

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


#include <QtWidgets>

#include "RGMainWindow.h"
#include "RGSettings.h"
#include "RGSettingsDialog.h"
#include "RGGoogleMap.h"
#include "RGVehicleList.h"
#include "RGEncVideo.h"
#include "RGEncFFmpeg.h"
#include "RGEncBmp2avi.h"
#include "RGMap.h"
#include "RGGPXReader.h"
#include "RGProjectReader.h"
#include "RGProjectWriter.h"
#include "RGRoute.h"
#include "RGRouteUi.h"
#include "RGViewWidget.h"
#include "RGUndoRedo.h"

#include "ui_routegen.h"

//Defined in main.cpp
extern const QString applicationName;

namespace  {
    void forceFileSuffix(QString &fileName, const QString &suffix)
    {
        if (QFileInfo (fileName).suffix().isEmpty())
        {
            fileName += "." + suffix;
        }

    }
}

RGMainWindow::RGMainWindow(QWidget *parent)
  :QMainWindow(parent),
     mVideoEncoder(nullptr)
{
  //Set currentPath
  QDir::setCurrent(QCoreApplication::applicationDirPath());

  setWindowIcon (QIcon(":/icons/icons/mapgen.png"));
  Ui::MainWindow ui;
  ui.setupUi(this);

  //Store some ui components locally
  actionOpen_image = ui.actionOpen_image;
  action_Quit = ui.action_Quit;
  actionSave_image = ui.actionSave_image;
  actionNew_project = ui.actionNew_project;
  actionSave_project = ui.actionSave_project;
  actionSave_project_as = ui.actionSave_project_as;
  actionOpen_project = ui.actionOpen_project;
  actionImport_Google_Map = ui.actionImport_Google_Map;
  actionImport_GPX = ui.actionImport_GPX;
  actionDraw_mode = ui.actionDraw_mode;
  actionNew_route = ui.actionNew_route;
  action_Undo = ui.action_Undo;
  action_Redo = ui.action_Redo;
  actionGenerate_map = ui.actionGenerate_map;
  actionPlayback = ui.actionPlayback;
  actionStop = ui.actionStop;

  action_Undo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
  action_Redo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Y));

  actionStop->setEnabled(false);
  actionGenerate_map->setEnabled(false);
  actionPlayback->setEnabled(false);
  actionSave_image->setEnabled(false);
  actionSave_project->setEnabled(false);
  actionSave_project_as->setEnabled(false);
  actionDraw_mode->setEnabled(false);
  actionNew_route->setEnabled(false);
  action_Undo->setEnabled(false);
  action_Redo->setEnabled(false);

  mResolutionCB = new QComboBox(ui.toolBar);
  mResolutionCB->addItem("8K: 7680x4320", QVariant(QSize(7680, 4320)));
  mResolutionCB->addItem("5K: 5120x2880", QVariant(QSize(5120, 2880)));
  mResolutionCB->addItem("4K: 4096x2160", QVariant(QSize(4096, 2160)));
  mResolutionCB->addItem("UHD 2160p: 3840x2160", QVariant(QSize(3840, 2160)));
  mResolutionCB->addItem("2.5K QHD 1440p: 2560x1440", QVariant(QSize(2560, 1440)));
  mResolutionCB->addItem("2K: 2048x1080", QVariant(QSize(2048, 1080)));
  mResolutionCB->addItem("FHD 1080p: 1920x1080", QVariant(QSize(1920, 1080)));
  mResolutionCB->addItem("HD 720p: 1280x720", QVariant(QSize(1280, 720)));
  mResolutionCB->addItem("SD: 720x576", QVariant(QSize(720, 576)));
  mResolutionCB->addItem("Custom", QVariant());
  mCustomResolutionItemIdx = 9;
  mResolutionCB->setToolTip("Select the output resolution of the generated video.");

  QSize selRes = RGSettings::getOutputResolution();
  int selResIdx = mResolutionCB->findData(selRes);
  if (selResIdx < 0)
  {
    //If idx not found, it's the custom resolution: set in as data of the Custom item
    mResolutionCB->setItemData(mCustomResolutionItemIdx, RGSettings::getOutputResolution());
    mResolutionCB->setItemText(mCustomResolutionItemIdx, QString("Custom: " ) +
                               QString::number(selRes.width()) + "x" + QString::number(selRes.height()));
    selResIdx = mCustomResolutionItemIdx;
  }
  mResolutionCB->setCurrentIndex(selResIdx);
  ui.toolBar->insertWidget(actionPlayback, mResolutionCB);

  //Video Encoder:
  initVideoEncoderFromSettings();

  mMap = new RGMap(this);

  //ViewWidget :
  mView = new RGViewWidget(mMap);
  ui.centralwidget->layout()->addWidget(mView);
  QObject::connect(actionStop, SIGNAL(triggered()),
                   mView, SLOT(stop()));
  QObject::connect(mView, SIGNAL(playbackStopped(bool)),
                   this, SLOT(enableGenerateActions(bool)));

  //Prepare status icons in status bar
  QPixmap checkPM(":/icons/icons/checkmark_green.svg");
  QPixmap globePM(":/icons/icons/globe.svg");
  mMapLoadedStatus = new QLabel();
  mMapGeoStatus = new QLabel();
  mRouteLoadedStatus = new QLabel();
  mRouteGeoStatus = new QLabel();
  statusBar()->addPermanentWidget(new QLabel(QStringLiteral("Map:")));
  statusBar()->addPermanentWidget(mMapLoadedStatus);
  statusBar()->addPermanentWidget(mMapGeoStatus);
  statusBar()->addPermanentWidget(new QLabel(QStringLiteral("Route:")));
  statusBar()->addPermanentWidget(mRouteLoadedStatus);
  statusBar()->addPermanentWidget(mRouteGeoStatus);
  mMapLoadedStatus->setPixmap(checkPM.scaledToHeight(20, Qt::SmoothTransformation));
  mMapGeoStatus->setPixmap(globePM.scaledToHeight(20, Qt::SmoothTransformation));
  mRouteLoadedStatus->setPixmap(checkPM.scaledToHeight(20, Qt::SmoothTransformation));
  mRouteGeoStatus->setPixmap(globePM.scaledToHeight(20, Qt::SmoothTransformation));
  mMapLoadedStatus->setEnabled(false);
  mMapGeoStatus->setEnabled(false);
  mRouteLoadedStatus->setEnabled(false);
  mRouteGeoStatus->setEnabled(false);
  mMapLoadedStatus->setToolTip("Enabled if a map is loaded");
  mMapGeoStatus->setToolTip("If the globe is enabled, the map has geographic (lat/lon) coordinates, so can be used to import a GPX route");
  mRouteLoadedStatus->setToolTip("Enabled if a route is loaded");
  mRouteGeoStatus->setToolTip("If the globe is enabled, the route is generated from (gpx) geographic coordinates");

  //Route :
  mRoute= new RGRoute(mMap);
  mRoute->setZValue(1);
  mRoute->setSmoothCoef(RGSettings::getSmoothLength());
  mRoute->setIconlessBeginEndFrames(RGSettings::getIconLessBeginEndFrames());
  QObject::connect(mRoute, SIGNAL(canGenerate(bool)),
                   this, SLOT(enableGenerateActions(bool)));

  mView->addRoute(mRoute);

  //Route UI (Toolbar with route controls):
  mRouteUi = new RGRouteUi();
  QObject::connect(mRouteUi,SIGNAL(penChanged(const QPen &)),mRoute,SLOT(changePen(const QPen &)));
  QObject::connect(mRouteUi,SIGNAL(smoothPathChecked(bool)),mRoute,SLOT(activateSmoothPath(bool)));
  QObject::connect(mRouteUi,SIGNAL(totalTimeChecked(bool)),mRoute,SLOT(activateTotalTime(bool)));
  QObject::connect(mRouteUi,SIGNAL(routeTimeChanged(int)),mRoute,SLOT(setRouteTime(int)));
  QObject::connect(mRouteUi,SIGNAL(vehicleChanged()),mRoute,SLOT(handleVehicleChange()));
  mRouteUi->setVehicleList(mRoute->getVehicleList());

  if (mMap)
  {
      QObject::connect(mMap, &RGMap::mapLoaded,
                       this, &RGMainWindow::handleMapLoaded);
  }

  QObject::connect(mResolutionCB, &QComboBox::currentIndexChanged,
                   this, &RGMainWindow::on_resolutionCBChanged);

  //set initial by sending signals
  mRouteUi->init();

  ui.mainLayout->insertWidget(0,mRouteUi);

  //Undo/Redo:
  mUndoRedo = new RGUndoRedo();
  QObject::connect(mRoute, SIGNAL(newUndoable(RGGraphicsObjectUndo *,QVariant)),
                   mUndoRedo, SLOT(addUndo(RGGraphicsObjectUndo *,QVariant)));
  QObject::connect(mUndoRedo, SIGNAL(undoPossible(bool)),
                   action_Undo, SLOT(setEnabled(bool)));
  QObject::connect(action_Undo, SIGNAL(triggered()),
                   mUndoRedo, SLOT(undo()));
  QObject::connect(mUndoRedo, SIGNAL(redoPossible(bool)),
                   action_Redo, SLOT(setEnabled(bool)));
  QObject::connect(action_Redo, SIGNAL(triggered()),
                   mUndoRedo, SLOT(redo()));

  setGeometry(RGSettings::getMainWindowGeometry());
}

void RGMainWindow::closeEvent(QCloseEvent *event)
{
    RGSettings::setMainWindowGeometry(geometry());

    if (checkMapSaveOrCancel())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }

}

void RGMainWindow::on_actionNew_project_triggered(bool)
{
    if (checkMapSaveOrCancel())
    {
        mRoute->clearPath(true);
        mMap->clearMap();
        mCurrentProjectFileName.clear();
    }
}

void RGMainWindow::on_actionOpen_project_triggered(bool)
{
    if (checkMapSaveOrCancel())
    {
        QString lastOpenDir = RGSettings::getLastOpenDir(RGSettings::RG_PROJECT_LOCATION);
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open RG project file"),
                                                      lastOpenDir,
                                                      tr("Project files (*.rgp)"));
        if (!fileName.isNull())
        {
            mRoute->clearPath(true);
            mMap->clearMap();
            mCurrentProjectFileName.clear();
            RGProjectReader rgReader(mRoute, mMap);
            if (!rgReader.readFile(fileName))
            {
                QMessageBox::warning (this, "Cannot read file", "Unable to open RG project file!");
            }
            else
            {
                RGSettings::setLastOpenDir(QFileInfo(fileName).absolutePath(), RGSettings::RG_PROJECT_LOCATION);
                mMap->resetDirty();
                mRoute->resetDirty();
                mCurrentProjectFileName = fileName;
            }
        }
    }
}

void RGMainWindow::on_actionOpen_image_triggered(bool /*checked*/)
{
    QString lastOpenDir = RGSettings::getLastOpenDir(RGSettings::RG_MAP_LOCATION);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                  lastOpenDir,
                                                  tr("Images (*.png *.bmp *.jpg *.tif *.gif)"));
    if (!fileName.isNull())
    {
        QPixmap pm(fileName);
        if (pm.isNull())
        {
            QMessageBox::critical (this, "Oops", "Could not load image");
        }
        else
        {
            RGSettings::setLastOpenDir(fileName, RGSettings::RG_MAP_LOCATION);
            //TODO: Validate that resolution of image is equeal or larger than the selected output resolution,
            //      give a warning if this is not the case!
            //      Also do this when importing Google map!
            mMap->loadMap(fileName, pm);
        }
    }
}

void RGMainWindow::on_actionSave_image_triggered(bool)
{
    QString lastSaveDir = RGSettings::getLastOpenDir(RGSettings::RG_MAP_LOCATION);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save map image as"),
                                                  QFileInfo(lastSaveDir).absoluteDir().absolutePath(),
                                                  tr("Images (*.png *.bmp *.jpg *.tif *.gif)"));


    if (!fileName.isNull())
    {
        if (QFileInfo (fileName).suffix().isEmpty())
        {
            fileName += ".png";
        }
        mView->saveRenderedImage(fileName, true);
        if (!mMap->saveGeoBoundsToNewFile(fileName) && mMap->hasGeoBounds())
        {
            QMessageBox::warning(this, "Cannot write geographic information", "Unable to save geographic (lat/lon) projection information,"
                                                                              "so the saved file cannot be used to import GPX routes! If the current"
                                                                              "loaded file is a GeoTIFF file, then use *.tif as extension.");
        }

        RGSettings::setLastOpenDir(fileName, RGSettings::RG_MAP_LOCATION);
    }
}

void RGMainWindow::on_actionSave_project_triggered(bool)
{
    if (mCurrentProjectFileName.isEmpty())
    {
        on_actionSave_project_as_triggered(true);
    }
    else
    {
        saveProjectFile(mCurrentProjectFileName);
    }
}

void RGMainWindow::on_actionSave_project_as_triggered(bool)
{
    QString lastSaveDir = RGSettings::getLastOpenDir(RGSettings::RG_PROJECT_LOCATION);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                    lastSaveDir,
                                                    tr("Projects (*.rgp)"));

    if (!fileName.isNull())
    {
        forceFileSuffix(fileName, "rgp");
        saveProjectFile(fileName);
    }
}

void RGMainWindow::on_actionPreferences_triggered(bool)
{
  RGSettingsDialog rgsettings(mVideoEncoder);

  if(rgsettings.exec()==QDialog::Accepted){
    mVideoEncoder->saveInSettings();
    mRoute->setSmoothCoef(rgsettings.getSmoothCoef());
    mRoute->setIconlessBeginEndFrames(rgsettings.getIconlessBeginEndFrames());

		if (RGSettings::getVideoEncoder() != mVideoEncoder->encoderName())
		{
			initVideoEncoderFromSettings();
		}
  }
  else
	{
    mVideoEncoder->updateFromSettings();
	}
}

void RGMainWindow::on_actionImport_Google_Map_triggered(bool)
{
    RGGoogleMap gm(this, mRoute->getGeoPath());
    if (gm.exec() == QDialog::Accepted)
    {
        QPixmap map = gm.getMap();
        if (map.isNull())
            return;

        //Yes, lastOpenDir, because lastSaveDir is used to save map files
        //from the main window
        QString lastSaveDir = RGSettings::getLastOpenDir(RGSettings::RG_MAP_LOCATION);
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                        lastSaveDir,
                                                        tr("Images (*.png *.bmp *.jpg *.tif *.gif)"));
        if (fileName.isEmpty())
        {
            return;
        }

        forceFileSuffix(fileName, "png");
        if (map.save(fileName))
        {
            RGSettings::setLastOpenDir(fileName, RGSettings::RG_MAP_LOCATION);
            mMap->loadMap(fileName, map, gm.getMapBounds());
        }
        else
        {
            QMessageBox::critical(this, "Cannot write file", "Unable to save map file!");
        }
    }
}

void RGMainWindow::on_actionImport_GPX_triggered(bool)
{
    QString lastSaveDir = RGSettings::getLastOpenDir(RGSettings::RG_GPX_LOCATION);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open GPX File"),
                                                    lastSaveDir,
                                                    tr("GPX files (*.gpx)"));
    if (!fileName.isNull())
    {
        RGSettings::setLastOpenDir(fileName, RGSettings::RG_GPX_LOCATION);
        RGGPXReader gpxReader(mRoute, mMap, this);
        if (gpxReader.readFile(fileName))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            QPixmap icon = msgBox.iconPixmap();
            QSize icSize = icon.size();
            icon.load(":/icons/icons/checkmark_green.svg");
            msgBox.setIconPixmap(icon.scaled(icSize));

            msgBox.setText("GXP route imported succesfully!");
            QPushButton *importButton = msgBox.addButton(tr("Import new map (Google maps)..."), QMessageBox::ActionRole);
            QPushButton *existingButton = msgBox.addButton(tr("Open existing map..."), QMessageBox::ActionRole);
            if (mMap->hasGeoBounds())
            {
                //Route loaded but map already has geo boundaries
                msgBox.setInformativeText(tr("Import new map, open other map, or draw route on current map?"));
                msgBox.addButton(tr("Keep current map"), QMessageBox::RejectRole);
            }
            else
            {
                //Route loaded but map has no geo boundaries
                msgBox.setInformativeText(tr("Unable to draw route on current map: import new (Google) map or open existing map?"));
                msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            }
            msgBox.exec();
            if (msgBox.clickedButton() == importButton)
            {
                on_actionImport_Google_Map_triggered(true);
            }
            else if (msgBox.clickedButton() == existingButton)
            {
                on_actionOpen_image_triggered(true);
            }
            updateStatusMessage();
        }
    }
}

void RGMainWindow::on_actionDraw_mode_triggered(bool checked)
{
  mView->stop();
  mRoute->setEditMode(checked);
  if(checked)
  {
      this->statusBar()->showMessage("hold SHIFT to record free drawing, CTRL to select several points. Del key to delete selected points");
  }
  else
  {
      this->statusBar()->clearMessage();
  }
}

void RGMainWindow::on_actionNew_route_triggered(bool)
{
    if(!actionDraw_mode->isChecked())
    {
        actionDraw_mode->trigger();
    }
    mRoute->clearPath(true);
    enableGenerateActions(false);
}

void RGMainWindow::on_actionPlayback_triggered(bool checked)
{
  Q_UNUSED(checked);
  mRoute->setEditMode(false);
  actionDraw_mode->setChecked(false);
  mView->play();
  mRouteUi->blockEssentialControls(true);
  actionStop->setEnabled(true);
  action_Undo->setEnabled(false);
  action_Redo->setEnabled(false);
}

void RGMainWindow::on_actionStop_triggered(bool checked)
{
  Q_UNUSED(checked);
  mView->stop();
}

void RGMainWindow::on_actionGenerate_map_triggered(bool checked)
{
    Q_UNUSED(checked);
    bool generateFramesOK = false;
    QString lastGenDir = RGSettings::getLastOpenDir(RGSettings::RG_MOVIE_LOCATION);

    QString dirInfoText = QString("Select an empty directory where the movie should be generated.");
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    dirInfoText,
                                                    lastGenDir,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if (dir.isNull())
    {
        return;
    }

    QString frameFilePrefix("map");
    QString frameFileType = "png";
    if (mVideoEncoder != nullptr && mVideoEncoder->exists())
    {
        frameFileType = mVideoEncoder->frameFileType();
    }

    //First generate the frame files into a directory that not already contains other image frames, if it does,
    //delete them first (after user confirmation)
    QDir qDir(dir);
    QStringList filters;
    filters << frameFilePrefix + "*" + frameFileType;
    qDir.setNameFilters(filters);
    QFileInfoList entries = qDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    if (entries.count() > 0)
    {
        QMessageBox::StandardButton answer = QMessageBox::question (this, "Folder not empty",
                              "Folder not empty, delete all existing frame image files in this folder first?",
                                         QMessageBox::Yes | QMessageBox::No |
                                         QMessageBox::Cancel, QMessageBox::No);
        if (answer == QMessageBox::Yes)
        {
            //Delete all files (or at least try to)
            for (QFileInfoList::iterator it = entries.begin(); it != entries.end(); ++it)
            {
                if (it->isWritable())
                {
                    qDebug() << "Deleting " + it->absoluteFilePath();
                    QFile::remove(it->absoluteFilePath());
                }
            }
        }
        else if (answer == QMessageBox::Cancel)
        {
            return;
        }
        //Continue
    }

    RGSettings::setLastOpenDir(dir, RGSettings::RG_MOVIE_LOCATION);
    //generate images :
    mRoute->setEditMode(false);

    generateFramesOK = mView->generateMovie(dir, frameFilePrefix, frameFileType, mGeneratedFrames);
    mRoute->setEditMode(actionDraw_mode->isChecked());

    if (generateFramesOK)
    {
            if (mVideoEncoder != nullptr && mVideoEncoder->exists())
			{
				blockUserInteraction(true);
				QObject::connect(mVideoEncoder,SIGNAL(movieGenerationFinished()),this,SLOT(movieGenerationFinished()));
                mVideoEncoder->generateMovie(dir);
			}
			else
			{
                QMessageBox::warning (this, "Encoder unavailable", "No encoder available, only image frames have been generated!");
			}
    }
}

void RGMainWindow::on_action_Tutorial_triggered(bool checked)
{
  Q_UNUSED(checked);
  QTextBrowser *te = new QTextBrowser ();
  te->setWindowTitle("Route Generator Tutorial");
  te->setWindowIcon (QIcon(":/icons/icons/mapgen.png")); 
  te->setAttribute(Qt::WA_DeleteOnClose);
  te->setSearchPaths(QStringList(QDir::currentPath()));
  te->setSource(QUrl("doc/tutorial.html"));
  te->resize(800,700);
  te->show();
}

void RGMainWindow::on_action_About_triggered(bool checked)
{
  Q_UNUSED(checked);
  QString txt = QString(
        "<html>"
        "<center>"
        "<p><b>") + applicationName + QString(" Copyright (C) 2008-2023  Michiel Jansen </b></p>"
                                              "<p>This program comes with ABSOLUTELY NO WARRANTY</p>"
                                              "This is free software, and you are welcome to redistribute it "
                                              "under certain conditions; see LICENSE file for details.</p>"
                                              "<p>This program was developed using the GPL version of Qt 5.15<br>"
                                              "(Copyright (C) 2008-2021  The Qt Company Ltd. All rights reserved),<br>"
                                              "Qt can be <a href=\"https://www.qt.io/download\"> downloaded </a>"
                                              "from the <a href=\"https://www.qt.io\">Qt</a> website. </p>"
                                              "<p>The conversion from BMP to AVI on Windows is provided by:<br>"
                                                                                            "<i>CODEX FFmpeg by Gyan Doshi<br>"
                                                                                            "See: <a href=\"https://www.gyan.dev/ffmpeg/builds/\">CODEX FFmpeg Builds Page</a></i><br>"
																							"OR<br>"
                                              "<i>bmp2avi Copyright (C)) Paul Roberts 1996 - 1998</i></p>"
                                              "<p>For more information or questions about Route Generator you can "
                                              "<a href=\"mailto:info@routegenerator.net\">contact</a> me by e-mail.</p>"
                                              "</center>"
                                              "</html>");
  QMessageBox::about (this, "About Route Generator", txt );
}


void RGMainWindow::on_action_Quit_triggered(bool checked)
{
    Q_UNUSED(checked);
    close();
}

void RGMainWindow::on_resolutionCBChanged(int index)
{
    if (index == mCustomResolutionItemIdx)
    {
        bool ok;
        QVariant customRes = mResolutionCB->itemData(index);
        QString defaultText = "1024x768";
        if (customRes.isValid())
        {
            QSize curRes = customRes.toSize();
            defaultText = QString::number(curRes.width()) + "x" + QString::number(curRes.height());
        }
        QString resTextLabel = tr("Enter custom resolution in format like: 1024x786");
        QString customResolutionText = QInputDialog::getText(this, tr("Enter custom resolution"),
                                               resTextLabel, QLineEdit::Normal,
                                               defaultText, &ok);
        if (ok && !customResolutionText.isEmpty())
        {
            QStringList resText = customResolutionText.split('x');
            ok = false;
            if (resText.length() == 2)
            {
                int xRes, yRes;
                xRes = resText[0].toInt(&ok);
                if (ok)
                {
                    yRes = resText[1].toInt(&ok);
                }
                if (ok)
                {
                    QSize customRes(xRes, yRes);
                    mResolutionCB->setItemData(mCustomResolutionItemIdx, customRes);
                    mResolutionCB->setItemText(mCustomResolutionItemIdx, QString("Custom: " ) + customResolutionText);
                }
            }
            if (!ok)
            {
                QMessageBox::warning(this, "Wrong resolution text", resTextLabel);
            }
        }
    }

    RGSettings::setOutputResolution(mResolutionCB->itemData(index).toSize());
}

void RGMainWindow::handleMapLoaded(const QPixmap &map)
{
    actionSave_image->setEnabled(!map.isNull());
    actionSave_project->setEnabled(!map.isNull());
    actionSave_project_as->setEnabled(!map.isNull());
    actionDraw_mode->setEnabled(!map.isNull());
    actionNew_route->setEnabled(!map.isNull());
    RGSettings::setLastOpenDir(mMap->fileName(), RGSettings::RG_MAP_LOCATION);

    updateStatusMessage();
}

void RGMainWindow::blockUserInteraction(bool busy)
{
    actionOpen_image->setEnabled(!busy);
    actionNew_project->setEnabled(!busy);
    actionOpen_project->setEnabled(!busy);
    action_Quit->setEnabled(!busy);
    actionSave_image->setEnabled(!busy);
    actionSave_project->setEnabled(!busy);
    actionSave_project_as->setEnabled(!busy);
    actionDraw_mode->setEnabled(!busy);
    actionNew_route->setEnabled(!busy);
    actionGenerate_map->setEnabled(!busy);
    actionPlayback->setEnabled(!busy);
    actionImport_Google_Map->setEnabled(!busy);
    actionImport_GPX->setEnabled(!busy);
    mRouteUi->blockEssentialControls(busy);
    //Stop is only enabled while playing back
    actionStop->setEnabled(false);
}

void RGMainWindow::enableGenerateActions(bool val)
{
    actionGenerate_map->setEnabled(val);
    actionPlayback->setEnabled(val);
    actionStop->setEnabled(false);
    mUndoRedo->sendActionSignals();

    //FIXME:
    //Route UI contains no "generate actions", so this is a hack to re-enable the controls in the route ui (to modify time, etc.),
    //when playback is finished (then val is true). In other cases (val == false) the controls should not be affected!
    //Currently the playback signal (triggering this slot) is emited by the view widget, but should be emitted by RGRoute
    if (val)
    {
        mRouteUi->blockEssentialControls(false);
    }
    updateStatusMessage();
}

void RGMainWindow::movieGenerationFinished()
{
  qDebug()<<"Movie generation finished";
  if (RGSettings::getDeleteBMPs()) {
    //Delete generated BMP's except for first and last frame (user might want to use them)
    for (int i = 1; i < mGeneratedFrames.size() - 1; i++) {
      if (!QFile::remove(mGeneratedFrames[i])) {
        QMessageBox::critical (this, "Error", "Unable to delete generated image frames! No permissions?");
        break;
      }
    }
  }
  blockUserInteraction(false);
  mVideoEncoder->disconnect();
}

void RGMainWindow::initVideoEncoderFromSettings()
{
    if ( mVideoEncoder != nullptr)
	{
		delete mVideoEncoder;
	}

#ifdef Q_OS_WIN
	QString vidEnc = RGSettings::getVideoEncoder();
	if (vidEnc == "bmp2avi")
	{
		mVideoEncoder = new RGEncBmp2avi();
	}
	else if (vidEnc == "ffmpeg")
	{
		mVideoEncoder = new RGEncFFmpeg();
	}
	else
	{
		QMessageBox::critical (this, "Error", tr("Unknown video encoder:") + vidEnc);
	}
#else
    mVideoEncoder = new RGEncFFmpeg();
#endif

    mVideoEncoder->initCodecExecutable();
    if(mVideoEncoder->exists())
    {
        qDebug()<<"encoder found !";
    }
}

void RGMainWindow::updateStatusMessage()
{   
    mMapLoadedStatus->setEnabled(!mMap->isEmpty());
    mMapGeoStatus->setEnabled(mMap->hasGeoBounds());
    mRouteLoadedStatus->setEnabled(!mRoute->isEmpty());
    mRouteGeoStatus->setEnabled(mRoute->hasGeoBounds());
}

bool RGMainWindow::checkMapSaveOrCancel()
{
    if (mMap->isDirty() || mRoute->isDirty())
    {
        QMessageBox::StandardButton answer = QMessageBox::question (this, tr("Project not saved"),
                                 tr("Do you want to save the current map and route as a project?"),
                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
        if (answer == QMessageBox::Cancel)
        {
            return false;
        }
        else
        {
            if (answer == QMessageBox::Yes)
            {
                on_actionSave_project_triggered(true);
            }
            return true;
        }
    }

    return true;
}

void RGMainWindow::saveProjectFile(const QString &projectFileName)
{
    RGProjectWriter projWriteer(mRoute, mMap, this);
    if (projWriteer.writeFile(projectFileName))
    {

        RGSettings::setLastOpenDir(QFileInfo(projectFileName).absolutePath(), RGSettings::RG_PROJECT_LOCATION);
        mMap->resetDirty();
        mRoute->resetDirty();
        mCurrentProjectFileName = projectFileName;
    }
    else
    {
        QMessageBox::critical(this, "Cannot write file", "Unable to write RG project file!");
    }
}
