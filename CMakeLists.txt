cmake_minimum_required(VERSION 2.8)

project (openvr_api)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_LIST_DIR})

set(openvr_source_files
	${CMAKE_CURRENT_LIST_DIR}/openvr_api_public.cpp
	${CMAKE_CURRENT_LIST_DIR}/hmderrors_public.cpp
)

find_package(osvr REQUIRED)
find_package(osvrRenderManager REQUIRED)
add_library(openvr_api SHARED ${openvr_source_files})
target_link_libraries(openvr_api PRIVATE m ${SDL2_LIBRARY} osvr::osvrClientKit osvrRenderManager::osvrRenderManagerCpp)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_DIR}/)

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
  openvr_api
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${SDL2_LIBRARIES}
  ${EXTRA_LIBS}
  dl
)
configure_file(cube_texture.png ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
