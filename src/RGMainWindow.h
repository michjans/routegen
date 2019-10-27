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

#ifndef RGMAIN_WINDOW_H
#define RGMAIN_WINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QStringList>

class QWidget;
class QTimer;
class QAction;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QPushButton;
class QLabel;
class RGEncVideo;
class RGMap;
class RGRoute;
class RGRouteUi;
class RGViewWidget;
class RGUndoRedo;

class RGMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  RGMainWindow(QWidget *parent = 0);

private slots:
  void on_actionOpen_image_triggered(bool );
  void on_actionSave_image_triggered(bool );
  void on_actionSave_project_triggered(bool );
  void on_actionPreferences_triggered(bool );
  void on_actionImport_Google_Map_triggered(bool );
  void on_actionImport_GPX_triggered(bool );
  void on_actionDraw_mode_triggered(bool );
  void on_actionNew_route_triggered(bool );
  void on_actionGenerate_map_triggered(bool );
  void on_actionPlayback_triggered(bool );
  void on_actionStop_triggered(bool );
  void on_action_Tutorial_triggered(bool );
  void on_action_About_triggered(bool );
  void on_action_Quit_triggered(bool );
  void handleMapLoaded(const QPixmap& map);
  
  void blockUserInteraction(bool);
  void enableGenerateActions(bool);
  void movieGenerationFinished();

private:
	void initVideoEncoderFromSettings();

  RGViewWidget *mView;
  RGEncVideo *mVideoEncoder;
  RGMap *mMap;
  RGRoute *mRoute;
  RGRouteUi *mRouteUi;

  RGUndoRedo  *mUndoRedo;

  QAction *actionOpen_image;
  QAction *action_Quit;
  QAction *actionSave_image;
  QAction *actionSave_project;
  QAction *actionImport_Google_Map;
  QAction *actionImport_GPX;
  QAction *actionDraw_mode;
  QAction *actionNew_route;
  QAction *action_Undo;
  QAction *action_Redo;
  QAction *actionGenerate_map;
  QAction *actionPlayback;
  QAction *actionStop;

  QStringList  mGeneratedBMPs;

};

#endif //MAIN_GUI_H
