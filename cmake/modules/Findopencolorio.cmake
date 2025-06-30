set(PCK "opencolorio")

if (${PCK}_FOUND)
  return()
endif()

find_path(${PCK}_INCLUDE_DIR
  NAMES OpenColorIO/OpenColorIO.h
  HINTS
    ${PRAGMA_DEPS_DIR}/opencolorio/include
)

find_library(${PCK}_LIBRARY
  NAMES OpenColorIO
  HINTS
    ${PRAGMA_DEPS_DIR}/opencolorio/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(${PCK}
  REQUIRED_VARS ${PCK}_LIBRARY ${PCK}_INCLUDE_DIR
)

if(${PCK}_FOUND)
  set(${PCK}_LIBRARIES   ${${PCK}_LIBRARY})
  set(${PCK}_INCLUDE_DIRS ${${PCK}_INCLUDE_DIR})
endif()
