# Copyright (C) 2006-2023 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_library(creo2urdf)
add_library(creo2urdf::creo2urdf ALIAS creo2urdf)

set(CREO2URDF_HDRS include/creo2urdf/Creo2Urdf.h
)
set(CREO2URDF_SRCS src/Creo2Urdf.cpp
                   src/main.cpp
)
set(CREO2URDF_IMPL_HDRS )
set(CREO2URDF_IMPL_SRCS )

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Source Files"
  FILES
    ${CREO2URDF_SRCS}
    ${CREO2URDF_IMPL_SRCS}
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES
    ${CREO2URDF_HDRS}
    ${CREO2URDF_IMPL_HDRS}
)

target_sources(creo2urdf
  PRIVATE
    ${CREO2URDF_SRCS}
    ${CREO2URDF_HDRS}
    ${CREO2URDF_IMPL_SRCS}
    ${CREO2URDF_IMPL_HDRS}
)


target_include_directories(creo2urdf
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(creo2urdf PUBLIC cxx_std_17)


# Link dependencies
# target_link_libraries(creo2urdf PUBLIC  Boost::boost
#                                         matioCpp::matioCpp
#                                         Threads::Threads
#                                 PRIVATE nlohmann_json::nlohmann_json)
# Support using filesystem on GCC < 9.1,
# see https://en.cppreference.com/w/cpp/filesystem#Notes
if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.1))
  target_link_libraries(creo2urdf PUBLIC stdc++fs)
endif()
## FIXME
list(APPEND CREO2URDF_PUBLIC_DEPS)
list(APPEND CREO2URDF_PRIVATE_DEPS)

set_target_properties(creo2urdf PROPERTIES DEFINE_SYMBOL CREO2URDF_EXPORTS)

set_property(TARGET creo2urdf PROPERTY PUBLIC_HEADER ${CREO2URDF_HDRS})
set_property(TARGET creo2urdf PROPERTY PRIVATE_HEADER ${CREO2URDF_IMPL_HDRS})
set_property(TARGET creo2urdf PROPERTY VERSION ${${CMAKE_PROJECT_NAME}_VERSION})
set_property(TARGET creo2urdf PROPERTY SOVERSION 0)
set_property(TARGET creo2urdf PROPERTY FOLDER "Libraries")

install(
  TARGETS creo2urdf
  EXPORT creo2urdf
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT creo2urdf
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT creo2urdf
    NAMELINK_COMPONENT creo2urdf-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT creo2urdf-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/creo2urdf"
    COMPONENT creo2urdf-dev
  PRIVATE_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/creo2urdf/impl"
    COMPONENT creo2urdf-priv-dev
)

set(CREO2URDF_PUBLIC_DEPS ${CREO2URDF_PUBLIC_DEPS} PARENT_SCOPE)
set(CREO2URDF_PRIVATE_DEPS ${CREO2URDF_PRIVATE_DEPS} PARENT_SCOPE)

include(InstallBasicPackageFiles)

install_basic_package_files(creo2urdf
                                COMPATIBILITY AnyNewerVersion
                                VERSION ${creo2urdf_VERSION}
                                DEPENDENCIES ${CREO2URDF_PUBLIC_DEPS}
                                PRIVATE_DEPENDENCIES ${CREO2URDF_PRIVATE_DEPS}
                                EXPORT_DESTINATION ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
