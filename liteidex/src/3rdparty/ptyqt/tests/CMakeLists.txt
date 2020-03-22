project(ptyqt-tests)

enable_testing()

find_package(Qt5Test REQUIRED)

add_executable(ptyqt_tests ptyqt_tests.cpp)
add_dependencies(ptyqt_tests ptyqt)
add_test(ptyqt_tests ptyqt_tests)

if (MSVC)
    if ("${PTYQT_DEBUG}")
        set(WINPTY_DEBUG_SERVER_PATH ${WINPTY_ROOT_DIR}/${TARGET_ARCH}/bin/winpty-debugserver.exe)
        add_definitions(-DWINPTY_DEBUG_SRV_PATH="${WINPTY_DEBUG_SERVER_PATH}")
        message("WinPty debug server path " ${WINPTY_DEBUG_SERVER_PATH})
    endif()
endif()

target_link_libraries(ptyqt_tests ptyqt Qt5::Core Qt5::Test)

if (MSVC)
    target_link_libraries(ptyqt_tests Qt5::Network ${WINPTY_LIBS})
endif()

foreach( file_i ${WINPTY_DIST_FILES})
    add_custom_command(
    TARGET ptyqt_tests
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${file_i} ${CMAKE_CURRENT_BINARY_DIR}
)
endforeach( file_i )

#file(COPY ${WINPTY_DIST_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
