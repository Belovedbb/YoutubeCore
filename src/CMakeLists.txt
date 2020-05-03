cmake_minimum_required(VERSION 3.8)

project(YoutubeSource)

set(SOURCES main.cpp)
set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
find_package(OpenSSL REQUIRED)

set(OPENSSL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/openssl)
include_directories(${OPENSSL_INCLUDE_DIR}) 

add_subdirectory(core)
add_executable(YoutubeSource ${SOURCES})

target_link_libraries(YoutubeSource core_tube OpenSSL::SSL OpenSSL::Crypto) 

install(TARGETS YoutubeSource DESTINATION ${INSTALL_BIN_DIR})