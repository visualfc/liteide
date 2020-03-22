cmake_minimum_required(VERSION 3.2.0)

if ($ENV{TRAVIS_BUILD_NUMBER})
    project(ptyqt VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER} LANGUAGES CXX)
else ($ENV{TRAVIS_BUILD_NUMBER})
    project(ptyqt VERSION 0.0.1 LANGUAGES CXX)
endif ($ENV{TRAVIS_BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

#params
#available params:
# - NO_BUILD_TESTS=1
# - NO_BUILD_EXAMPLES=1
# - WINPTY_LIBS=<path/to/winpty.lib> - redefine path to winpty lib for libkage and ignore included find mechanic
# - WINPTY_DBGLIBS=<path/to/debug/winpty.lib> - redefine path to winpty lib for libkage and ignore included find mechanic
IF("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(PTYQT_DEBUG TRUE)
    add_definitions(-DPTYQT_DEBUG)
else()
    set(PTYQT_DEBUG FALSE)
endif()

set(BUILD_TYPE STATIC) #STATIC / SHARED

if (MSVC)
    #message("PRE_TARGET_ARCH ${TARGET_ARCH}")
    #message("VCPKG_TARGET_TRIPLET ${VCPKG_TARGET_TRIPLET}")

    #target arch for find winpty libs
    if ("${TARGET_ARCH}" STREQUAL "x86_amd64")
        set(TARGET_ARCH x64)
    endif()
	if ("${VCPKG_TARGET_TRIPLET}" MATCHES "x86")
        set(TARGET_ARCH x86) #x86 / x64
    endif()
    if ("${TARGET_ARCH}" STREQUAL "")
        set(TARGET_ARCH x64) #x86 / x64
    endif()
    set(WINPTY_ROOT_DIR "") #user can define his own installation of WinPty for prevent auto-downloading dependency

    #message("POST_TARGET_ARCH ${TARGET_ARCH}")

    if("${BUILD_TYPE}" STREQUAL "STATIC")
        set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
        include(CMakeMsvc)
        configure_msvc_runtime ("${VCPKG_TARGET_TRIPLET}")
    endif()

endif()

#dependencies
find_package(Qt5Core REQUIRED)

if (MSVC)
    #for debug build we can redefine winpty libs to debug version
    if (CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT ${WINPTY_DBGLIBS} STREQUAL "")
        set(WINPTY_LIBS ${WINPTY_DBGLIBS})
    endif()

    add_subdirectory(thirdparty)
    find_package(Qt5Network REQUIRED)

    message("WinPty root dir " ${WINPTY_ROOT_DIR})
    message("WinPty libs " ${WINPTY_LIBS})
endif()

#install
#set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR})

set(PTYQT_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(PTYQT_INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(PTYQT_INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)

set(PTYQTLIB_HEADERS_DIR ${PROJECT_SOURCE_DIR}/core)

include_directories(${PTYQT_INSTALL_INCLUDE_DIR})
include_directories(${PTYQTLIB_HEADERS_DIR})
if (MSVC)
    include_directories(${WINPTY_ROOT_DIR}/include)
endif()

#sub projects
add_subdirectory(core)

if (NOT "${NO_BUILD_TESTS}" STREQUAL "1")
  enable_testing()
  add_subdirectory(tests)
endif()

if (NOT "${NO_BUILD_EXAMPLES}" STREQUAL "1")
    add_subdirectory(examples)
endif()
