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
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Make sure that libraries can be found in ./lib sub-directory (to prevent having to set LD_LIBRARY_PATH)
# NOTE:This variable is only processed during a 'make install'!
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Positioning WebEngineWidgets Widgets)
find_package(GeoTIFF REQUIRED)
find_package(TIFF REQUIRED)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  enable_language("RC")
  set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/routegen.rc)
endif()

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
    RGMapProjection.cpp RGMapProjection.h
    RGGoogleMapProjection.cpp RGGoogleMapProjection.h
    RGGeoTiffMapProjection.cpp RGGeoTiffMapProjection.h
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
    routegen.qrc
    ${WIN32_RESOURCES}
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
    geotiff_library
)

set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold ${CMAKE_CXX_FLAGS}")

# Qt5LinguistTools
#find_package(Qt5LinguistTools)
#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} routegen_en.ts routegen_nl.ts)

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
