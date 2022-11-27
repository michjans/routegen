cmake_minimum_required(VERSION 3.16)
project(routegen VERSION 1.11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Positioning WebEngineWidgets Widgets)

add_executable(routegen WIN32 MACOSX_BUNDLE
    GPXSelectionDialog.cpp GPXSelectionDialog.h
    RGEditPath.cpp RGEditPath.h
    RGEditPathPoint.cpp RGEditPathPoint.h
    RGEncBmp2avi.cpp RGEncBmp2avi.h
    RGEncFFmpeg.cpp RGEncFFmpeg.h
    RGEncVideo.cpp RGEncVideo.h
    RGGPXReader.cpp RGGPXReader.h
    RGGoogleMap.cpp RGGoogleMap.h
    RGGraphicsObjectUndo.cpp RGGraphicsObjectUndo.h
    RGMainWindow.cpp RGMainWindow.h
    RGMap.cpp RGMap.h
    RGMapBounds.h
    RGPath.cpp RGPath.h
    RGProjectReader.cpp RGProjectReader.h
    RGProjectWriter.cpp RGProjectWriter.h
    RGReader.cpp RGReader.h
    RGRoute.cpp RGRoute.h
    RGRouteUi.cpp RGRouteUi.h
    RGSettings.cpp RGSettings.h
    RGSettingsDialog.cpp RGSettingsDialog.h
    RGUndoRedo.cpp RGUndoRedo.h
    RGVehicle.cpp RGVehicle.h
    RGVehicleDialog.cpp RGVehicleDialog.h
    RGVehicleList.cpp RGVehicleList.h
    RGVehicleOriginPt.cpp RGVehicleOriginPt.h
    RGViewWidget.cpp RGViewWidget.h
    RGWriter.cpp RGWriter.h
    googlemap.ui
    gpxselectiondialog.ui
    main.cpp
    routegen.ui
    routeui.ui
    settings.ui
    vehicledialog.ui
    videosettings.ui
)
target_compile_definitions(routegen PRIVATE
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
)

target_link_libraries(routegen PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Positioning
    Qt::WebEngineWidgets
    Qt::Widgets
)


# Resources:
set(routegen_resource_files
    "icons/action_filenew.svg"
    "icons/checkmark_green.svg"
    "icons/desktop-32.png"
    "icons/draw-path.png"
    "icons/exit.png"
    "icons/fileopen.png"
    "icons/generate.png"
    "icons/globe.svg"
    "icons/google-maps-logo.png"
    "icons/gps.png"
    "icons/map_pencil.svg"
    "icons/map_pencil_save.svg"
    "icons/mapgen.png"
    "icons/media-floppy.svg"
    "icons/new-route.png"
    "icons/player_play.png"
    "icons/player_stop.png"
    "icons/redo.png"
    "icons/reset.png"
    "icons/save-project.png"
    "icons/settings.png"
    "icons/undo.png"
)

qt_add_resources(routegen "routegen"
    PREFIX
        "/icons"
    FILES
        ${routegen_resource_files}
)

set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold ${CMAKE_CXX_FLAGS}")

if(CMAKE_BUILD_TYPE STREQUAL Release)
    target_compile_definitions(routegen PRIVATE
        QT_NO_DEBUG_OUTPUT
    )
endif()

install(TARGETS routegen
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
