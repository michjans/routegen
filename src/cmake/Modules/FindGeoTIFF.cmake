# FindGeoTIFF.cmake to find GeoTIFF dev library on Linux, because libgeotiff-dev
# does not provde the cmake modules itself.
find_path(GEOTIFF_INCLUDE_DIR geotiff.h
          PATH_SUFFIXES geotiff)

find_library(GEOTIFF_LIBRARY NAMES geotiff)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GeoTIFF DEFAULT_MSG
    GEOTIFF_LIBRARY GEOTIFF_INCLUDE_DIR)

if(GeoTIFF_FOUND)
    set(GeoTIFF_INCLUDE_DIRS ${GEOTIFF_INCLUDE_DIR})
    set(GeoTIFF_LIBRARIES ${GEOTIFF_LIBRARY})
endif()

