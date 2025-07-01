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

set(REQ_VARS ${PCK}_LIBRARY ${PCK}_UTIL_LIBRARY ${PCK}_INCLUDE_DIR)
if(WIN32)
  find_file(
    ${PCK}_OIIO_RUNTIME
    NAMES OpenImageIO.dll
    HINTS
      ${PRAGMA_DEPS_DIR}/openimageio/bin
  )
  find_file(
    ${PCK}_OIIO_UTIL_RUNTIME
    NAMES OpenImageIO_Util.dll
    HINTS
      ${PRAGMA_DEPS_DIR}/openimageio/bin
  )
  set(REQ_VARS ${REQ_VARS} ${PCK}_OIIO_RUNTIME ${PCK}_OIIO_UTIL_RUNTIME)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(${PCK}
  REQUIRED_VARS ${REQ_VARS}
)

if(${PCK}_FOUND)
  set(${PCK}_LIBRARIES   ${${PCK}_LIBRARY} ${${PCK}_UTIL_LIBRARY})
  set(${PCK}_INCLUDE_DIRS ${${PCK}_INCLUDE_DIR})
  if(WIN32)
    set(${PCK}_RUNTIME   ${${PCK}_OIIO_RUNTIME} ${${PCK}_OIIO_UTIL_RUNTIME})
  endif()
endif()
