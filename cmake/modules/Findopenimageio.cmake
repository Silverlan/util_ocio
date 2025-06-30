set(PCK "openimageio")

if (${PCK}_FOUND)
  return()
endif()

find_path(${PCK}_INCLUDE_DIR
  NAMES OpenImageIO/oiioversion.h
  HINTS
    ${PRAGMA_DEPS_DIR}/openimageio/include
)

find_library(${PCK}_LIBRARY
  NAMES OpenImageIO
  HINTS
    ${PRAGMA_DEPS_DIR}/openimageio/lib
)

find_library(${PCK}_UTIL_LIBRARY
  NAMES OpenImageIO_Util
  HINTS
    ${PRAGMA_DEPS_DIR}/openimageio/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(${PCK}
  REQUIRED_VARS ${PCK}_LIBRARY ${PCK}_UTIL_LIBRARY ${PCK}_INCLUDE_DIR
)

if(${PCK}_FOUND)
  set(${PCK}_LIBRARIES   ${${PCK}_LIBRARY} ${${PCK}_UTIL_LIBRARY})
  set(${PCK}_INCLUDE_DIRS ${${PCK}_INCLUDE_DIR})
endif()
