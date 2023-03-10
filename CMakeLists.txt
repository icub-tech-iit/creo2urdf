# Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.13)
project(creo2urdf LANGUAGES C CXX
                  VERSION 0.0.0)

include(GNUInstallDirs)
include(FeatureSummary)

find_package(YCM 0.12 REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Control where libraries and executables are placed during the build.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")


# FIXME THIS COULD GIVE PROBLEMS
# Under MSVC, we set CMAKE_DEBUG_POSTFIX to "d" to add a trailing "d" to library
# built in debug mode.
#if(MSVC)
#  set(CMAKE_DEBUG_POSTFIX "d")
#endif()

# To build shared libraries in Windows, we set CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS to TRUE.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(BUILD_SHARED_LIBS ON)
# Build position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD") # FIXME /MDd does not work, probably because ptc libraries are compiled in release

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.19)
  cmake_policy(SET CMP0111 NEW)
endif()


# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"       # Libraries
                          BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"       # Binaries
                                   "${CMAKE_INSTALL_FULL_LIBDIR}/creo2urdf"  # Plugins
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

# Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif()
endif()

#### Dependencies

# FIXME add find_package CREO9

if (NOT "$ENV{CREO9_INSTALL_PATH}" STREQUAL "")
  set(CREO9_INSTALL_PATH "$ENV{CREO9_INSTALL_PATH}" CACHE INTERNAL "Copied from environment variable CREO9_INSTALL_PATH")
else()
  message( FATAL_ERROR "CREO9_INSTALL_PATH not set" )
endif()

message("CREO9_INSTALL_PATH = ${CREO9_INSTALL_PATH}")

#### Optional Dependencies

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES)

add_subdirectory(src)

option(BUILD_EXAMPLES "Build the examples" ON)
option(BUILD_TESTING "Create tests using CMake" OFF)

##############################
########### Test #############
##############################

set_property(GLOBAL PROPERTY USE_FOLDERS 1)

include(AddUninstallTarget)
