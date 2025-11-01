# FindGeoTIFF.cmake
# Works for Linux and macOS (including Homebrew on Apple Silicon)

find_path(GEOTIFF_INCLUDE_DIR
    NAMES geotiff.h
    PATH_SUFFIXES geotiff
    PATHS
        /usr/include
        /usr/local/include
        /opt/homebrew/include
)

find_library(GEOTIFF_LIBRARY
    NAMES geotiff
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
        /opt/homebrew/opt/libgeotiff/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GeoTIFF DEFAULT_MSG
    GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR
)

if(GeoTIFF_FOUND)
    set(GeoTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
    set(GeoTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GeoTIFF DEFAULT_MSG
    GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR
)

if(GeoTIFF_FOUND)
    set(GeoTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
    set(GeoTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
    message(STATUS "GeoTIFF include dir found: ${GEOTIFF_INCLUDE_DIR}")
endif()
