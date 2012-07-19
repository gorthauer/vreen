project(vkit)
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if(COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
endif(COMMAND cmake_policy)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(QT_USE_QTCORE true)
set(QT_USE_QTNETWORK true)
#set(CMAKE_AUTOMOC true)

find_package(Qt4 REQUIRED)
include(UseQt4)
include(CommonUtils)

option(VKIT_WITH_OAUTH "Enable webkit based oauth authorization" ON)
option(VKIT_WITH_QMLAPI "Enable QtQuick bindings for vkit" ON)
option(VKIT_INSTALL_HEADERS "Install devel headers for vkit" ON)

#TODO check if vars is defined
set(RLIBDIR bin)
set(LIBDIR lib${LIB_SUFFIX})
set(LIB_DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})

set(CMAKE_LIBVK_VERSION_MAJOR 0 CACHE INT "Major vk version number" FORCE)
set(CMAKE_LIBVK_VERSION_MINOR 9 CACHE INT "Minor vk version number" FORCE)
set(CMAKE_LIBVK_VERSION_PATCH 0 CACHE INT "Release vk version number" FORCE)
set(CMAKE_LIBVK_VERSION_STRING "${CMAKE_LIBVK_VERSION_MAJOR}.${CMAKE_LIBVK_VERSION_MINOR}.${CMAKE_LIBVK_VERSION_PATCH}" CACHE STRING "vkit version string" FORCE)

add_subdirectory(src)
