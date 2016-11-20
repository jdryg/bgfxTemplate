cmake_minimum_required(VERSION 3.3)
project(bgfxTemplate)

add_definitions(-DBGFX_CONFIG_MULTITHREADED=0)
if(APPLE)
    add_definitions(-DBGFX_CONFIG_RENDERER_OPENGL=32)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/main.cpp)

IF(APPLE)
    set(BGFX bgfx/src/amalgamated.mm)
ELSE()
    set(BGFX bgfx/src/amalgamated.cpp)
ENDIF()

include_directories(include
        bx/include
        bgfx/include
        bgfx/3rdparty
        bgfx/3rdparty/khronos)

IF(APPLE)
    include_directories(bx/include/compat/osx)
    link_directories(lib/osx)
ELSE()
    link_directories(lib/linux)
ENDIF()

if(NOT APPLE)
    add_executable(bgfxTemplate ${SOURCE_FILES} ${BGFX})
else()
    add_executable(bgfxTemplate MACOSX_BUNDLE ${SOURCE_FILES} ${BGFX})
endif()

IF(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_gl_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    target_link_libraries(bgfxTemplate glfw3 ${COCOA_LIBRARY} ${OPENGL_gl_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
ELSE()
    target_link_libraries(bgfxTemplate glfw3 GL X11 Xi Xrandr Xxf86vm Xinerama Xcursor rt m pthread dl)
ENDIF()

set_target_properties(bgfxTemplate PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin
        COMPILE_DEFINITIONS_DEBUG BGFX_CONFIG_DEBUG=1)

