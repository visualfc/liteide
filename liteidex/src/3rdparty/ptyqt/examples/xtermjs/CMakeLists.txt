project(ptyqt-xtermjs)

find_package(Qt5WebSockets CONFIG REQUIRED)

add_executable(xtermjs_sample xtermjs.cpp)
add_dependencies(xtermjs_sample ptyqt)

target_link_libraries(xtermjs_sample ptyqt Qt5::Core ${Qt5WebSockets_LIBRARIES})

if (MSVC)
    target_link_libraries(xtermjs_sample Qt5::Network ${WINPTY_LIBS})
endif()

foreach( file_i ${WINPTY_DIST_FILES})
    add_custom_command(
    TARGET xtermjs_sample
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${file_i} ${CMAKE_CURRENT_BINARY_DIR}
)
endforeach( file_i )
