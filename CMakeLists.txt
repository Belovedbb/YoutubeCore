# CMakeList.txt : CMake project for YoutubeCore, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (YoutubeCore CXX)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 11) 

set(CMAKE_CXX_FLAG "${CMAKE_CXX_FLAG} -std=c++11 -03")

set (warnings "-Wall -Wextra -Werror")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/build)
set(INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
set(INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(HTTPLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/httplib)


#set httplib protocol header
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()
add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT)

include_directories(${INSTALL_INCLUDE_DIR}) 
include_directories(${HTTPLIB_INCLUDE_DIR}) 

# Add source to this project's executable.
add_subdirectory(src)
add_subdirectory(test)

# TODO: Add tests and install targets if needed.