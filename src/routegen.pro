#-------------------------------------------------
#
# Project created by QtCreator 2019-05-17T13:05:17
#
#-------------------------------------------------

QT += core gui
QT += widgets
QT += webenginewidgets
QT += positioning

TARGET = routegen
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += debug_and_release
CONFIG += c++11

# CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp RGMainWindow.cpp RGVehicleDialog.cpp RGSettings.cpp RGSettingsDialog.cpp RGGoogleMap.cpp \
    GPXSelectionDialog.cpp \
    RGGPXReader.cpp \
    RGReader.cpp \
    RGVehicle.cpp \
    RGVehicleList.cpp \
    RGEncVideo.cpp \
    RGEncFFmpeg.cpp \
    RGEncBmp2avi.cpp \
    RGRouteUi.cpp \
    RGRoute.cpp \
    RGPath.cpp \
    RGViewWidget.cpp \
    RGEditPathPoint.cpp \
    RGEditPath.cpp \
    RGVehicleOriginPt.cpp \
    RGUndoRedo.cpp \
    RGGraphicsObjectUndo.cpp \
    RGWriter.cpp \
    RGProjectReader.cpp \
    RGProjectWriter.cpp \
    RGMap.cpp
	
RESOURCES += routegen.qrc

HEADERS += RGMainWindow.h RGVehicleDialog.h RGSettings.h RGSettingsDialog.h RGGoogleMap.h \
    GPXSelectionDialog.h \
    RGGPXReader.h \
    RGMapBounds.h \
    RGReader.h \
    RGVehicle.h \
    RGVehicleList.h \
    RGEncVideo.h \
    RGEncFFmpeg.h \
    RGEncBmp2avi.h \
    RGRouteUi.h \
    RGRoute.h \
    RGPath.h \
    RGViewWidget.h \
    RGEditPathPoint.h \
    RGEditPath.h \
    RGVehicleOriginPt.h \
    RGUndoRedo.h \
    RGGraphicsObjectUndo.h \
    RGWriter.h \
    RGProjectReader.h \
    RGProjectWriter.h \
    RGMap.h

FORMS += routegen.ui settings.ui googlemap.ui \
    gpxselectiondialog.ui \
    vehicledialog.ui \
    videosettings.ui \
    routeui.ui

win32 {
  RC_FILE = routegen.rc
} 

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    message( "release" )
}
CONFIG(debug, debug|release) {
    CONFIG += debug
}
	
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
