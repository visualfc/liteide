project(ptyqt-core VERSION 0.1 LANGUAGES CXX)

set(SOURCE_FILES
    ptyqt.h
    ptyqt.cpp
    iptyprocess.h
)

if (MSVC)
    set(SOURCE_FILES
        ${SOURCE_FILES}
        winptyprocess.h
        winptyprocess.cpp
        conptyprocess.h
        conptyprocess.cpp
        )
else()
    set(SOURCE_FILES
        ${SOURCE_FILES}
        unixptyprocess.cpp
        unixptyprocess.h
        )
endif()

if("${BUILD_TYPE}" STREQUAL "STATIC")
    add_library( ptyqt STATIC ${SOURCE_FILES} )
    add_definitions(-DPTYQT_BUILD_STATIC)
else()
    add_library( ptyqt SHARED ${SOURCE_FILES} )
    add_definitions(-DPTYQT_BUILD_DYNAMIC)
endif()

set(ADDITIONAL_LIBS "")

#disable this on Travis?AppVeyor
if (NOT BUILD_SERVER)
    find_library(PCRE2_LIBRARIES NAMES pcre2 pcre2-16)
    find_path(PCRE2_INCLUDE_DIRS pcre2.h)
    if(PCRE2_LIBRARIES AND PCRE2_INCLUDE_DIRS)
      message(STATUS "PCRE2 libs: ${PCRE2_LIBRARIES}")
      message(STATUS "PCRE2 include directory: ${PCRE2_INCLUDE_DIRS}")
      set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)
      #add_custom_target(pcre2)
    else()
      set(PCRE2_FOUND FALSE CACHE BOOL "Found PCRE2 libraries" FORCE)
      message(STATUS "PCRE2 library not found.")
    endif()

    find_package(OpenSSL REQUIRED)
    find_package(BZip2 REQUIRED)
    find_package(ZLIB REQUIRED)
    find_library(DBLCONV_LIBRARIES NAMES pcre2 double-conversion)

    list(APPEND ADDITIONAL_LIBS ${PCRE2_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto ${BZIP2_LIBRARIES} ${ZLIB_LIBRARIES} ${DBLCONV_LIBRARIES})

    if (MSVC)
        list(APPEND ADDITIONAL_LIBS wsock32 ws2_32 crypt32 iphlpapi netapi32 version winmm userenv)
    elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin"
            OR (UNIX AND APPLE)
            OR "${CMAKE_CXX_COMPILER}" MATCHES "/usr/bin/clang"
            )

        set(LIBS_MACOS
            "-framework Security -framework AppKit -framework CoreFoundation -framework IOKit"
            "-framework CoreGraphics -framework CFNetwork -framework CoreText -framework Carbon"
            "-framework CoreServices -framework ApplicationServices -framework SystemConfiguration"
        )

        list(APPEND ADDITIONAL_LIBS ${LIBS_MACOS})

    elseif(UNIX AND NOT APPLE)
        set(LIBS_LINUX 
		    "-lpthread -ldl -static-libstdc++"
		)
		list(APPEND ADDITIONAL_LIBS ${LIBS_LINUX})
    endif()
endif()

target_link_libraries(ptyqt Qt5::Core ${ADDITIONAL_LIBS})

if (MSVC)
    target_link_libraries( ptyqt Qt5::Network ${WINPTY_LIBS})
endif()

install(TARGETS ptyqt DESTINATION ${PTYQT_INSTALL_LIB_DIR})
install(FILES ptyqt.h iptyprocess.h DESTINATION ${PTYQT_INSTALL_INCLUDE_DIR})
