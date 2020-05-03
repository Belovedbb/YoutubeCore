cmake_minimum_required(VERSION 3.8)
set(CMAKE_BUILD_TYPE Debug)


project(core_tube)

set(SOURCES tube.cpp youtube_interface.cpp extracts.cpp split_url.cpp parse_url.cpp downloader.cpp)

add_library(core_tube SHARED STATIC ${SOURCES})

install(TARGETS core_tube DESTINATION ${INSTALL_LIB_DIR})