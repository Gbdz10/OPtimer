cmake_minimum_required(VERSION 3.16)
project(OPtimer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(APP_RES "")
if(WIN32)
    set(APP_RES app.rc)
endif()

add_executable(OPtimer src/main.cpp ${APP_RES})

if(WIN32)
    # Đang build cho Windows (có mingw-toolchain.cmake)
    set(RAYLIB_WIN_DIR "$ENV{HOME}/raylib-win/src")
    target_include_directories(OPtimer PRIVATE ${RAYLIB_WIN_DIR})
    target_link_libraries(OPtimer PRIVATE
        ${RAYLIB_WIN_DIR}/libraylib.a winmm gdi32 opengl32)
    target_link_options(OPtimer PRIVATE -static -static-libgcc -static-libstdc++ -mwindows)
else()
    # Build cho Linux
    set(ENV{PKG_CONFIG_PATH} "/usr/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)
    target_include_directories(OPtimer PRIVATE ${RAYLIB_INCLUDE_DIRS})
    target_link_directories(OPtimer PRIVATE ${RAYLIB_LIBRARY_DIRS})
    target_link_libraries(OPtimer PRIVATE raylib GL m pthread dl rt X11)
endif()
