include(${CMAKE_SOURCE_DIR}/cmake/pr_common.cmake)

set(PROJ_NAME util_ocio)
pr_add_library(${PROJ_NAME} STATIC)

pr_add_dependency(${PROJ_NAME} util_image TARGET PUBLIC)

find_package(opencolorio REQUIRED)
pr_add_external_dependency(${PROJ_NAME} opencolorio LIBRARY PRIVATE)

find_package(openimageio REQUIRED)
pr_add_external_dependency(${PROJ_NAME} openimageio LIBRARY PRIVATE)

pr_init_module(${PROJ_NAME})

if(UNIX)
	target_link_libraries(${PROJ_NAME} PRIVATE dl)
endif()

pr_finalize(${PROJ_NAME})
