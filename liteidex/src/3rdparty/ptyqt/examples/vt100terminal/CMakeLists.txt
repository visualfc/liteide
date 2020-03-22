project(ptyqt-vt100terminal)

include_directories(qvterminal/src/)

set(VTLIB_SOURCE_FILES
    qvterminal/src/qvtchar.cpp
    qvterminal/src/qvtchar.h
    qvterminal/src/qvtcharformat.cpp
    qvterminal/src/qvtcharformat.h
    qvterminal/src/qvterminal.cpp
    qvterminal/src/qvterminal.h
    qvterminal/src/qvtlayout.cpp
    qvterminal/src/qvtlayout.h
    qvterminal/src/qvtline.cpp
    qvterminal/src/qvtline.h
)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

#add_library( qvterminal STATIC ${VTLIB_SOURCE_FILES} )
#qt5_use_modules(qvterminal Widgets)

add_executable(vt100terminal_sample vt100terminal.cpp ptydevice.h ${VTLIB_SOURCE_FILES})
add_dependencies(vt100terminal_sample ptyqt)
qt5_use_modules(vt100terminal_sample Widgets)

target_link_libraries(vt100terminal_sample ptyqt Qt5::Core)

if (MSVC)
    add_dependencies(vt100terminal_sample winpty)
    target_link_libraries(vt100terminal_sample Qt5::Network ${WINPTY_LIBS})
endif()

foreach( file_i ${WINPTY_DIST_FILES})
    add_custom_command(
    TARGET vt100terminal_sample
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${file_i} ${CMAKE_CURRENT_BINARY_DIR}
)
endforeach( file_i )
