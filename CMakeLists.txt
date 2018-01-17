cmake_minimum_required(VERSION 2.8)

project (openvr_api)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_LIST_DIR})

set(openvr_source_files
	${CMAKE_CURRENT_LIST_DIR}/openvr_api_public.cpp
	${CMAKE_CURRENT_LIST_DIR}/hmderrors_public.cpp
)


OPTION(OSVR_LIBOPENVR "OSVR Backend" OFF)
OPTION(OPENHMD_LIBOPENVR "OpenVR Backend" ON)

add_library(openvr_api SHARED ${openvr_source_files})

set(BACKEND_LIBS "")

if (OSVR_LIBOPENVR)
    add_definitions(-DBUILD_OSVR)
    find_package(osvr REQUIRED)
    find_package(osvrRenderManager REQUIRED)
    set (BACKEND_LIBS ${BACKEND_LIBS} osvr::osvrClientKit osvrRenderManager::osvrRenderManagerCpp)
endif(OSVR_LIBOPENVR)

if (OPENHMD_LIBOPENVR)
    add_definitions(-DBUILD_OPENHMD)
    include_directories("${CMAKE_SOURCE_DIR}/external/openhmd/include")
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/external/openhmd/cmake/")
    add_subdirectory(./external/openhmd)
    set (BACKEND_LIBS ${BACKEND_LIBS} openhmd-shared)
    add_dependencies(openvr_api openhmd-shared)
endif(OPENHMD_LIBOPENVR)

target_link_libraries(openvr_api PRIVATE m ${SDL2_LIBRARIES} ${BACKEND_LIBS})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
string(REPLACE "libGLEWd.so" "libGLEW.so" GLEW_LIBRARIES "${GLEW_LIBRARIES}")
add_definitions(-DLINUX -DPOSIX)
file(GLOB SHARED_SRC_FILES
  ./shared/*.cpp
  ./shared/*.h
)
add_executable(hellovr_opengl
  ${SHARED_SRC_FILES}
  hellovr_opengl_main.cpp
)
target_link_libraries(hellovr_opengl
  openvr_api #TODO: build one for each
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${EXTRA_LIBS}
  dl
)
configure_file(cube_texture.png ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
