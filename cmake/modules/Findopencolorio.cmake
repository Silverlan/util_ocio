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

set(REQ_VARS ${PCK}_LIBRARY ${PCK}_INCLUDE_DIR)
if(WIN32)
  find_file(
    ${PCK}_RUNTIME
    NAMES OpenColorIO_2_4.dll
    HINTS
      ${PRAGMA_DEPS_DIR}/opencolorio/bin
  )
  set(REQ_VARS ${REQ_VARS} ${PCK}_RUNTIME)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(${PCK}
  REQUIRED_VARS ${REQ_VARS}
)

if(${PCK}_FOUND)
  set(${PCK}_LIBRARIES   ${${PCK}_LIBRARY})
  set(${PCK}_INCLUDE_DIRS ${${PCK}_INCLUDE_DIR})
endif()
