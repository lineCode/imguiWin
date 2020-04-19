cmake_minimum_required(VERSION 3.15)

# set the project name
project(imguiWin)

set(SRC_DIR src)
set(IMGUI_DIR ${SRC_DIR}/imgui)
set(GL3W_DIR ${SRC_DIR}/GL)
set(IMGUI_BIND_DIR ${SRC_DIR}/imgui-bindings)
set(INCLUDE_DIR include)
set(SDL2_DIR ${SRC_DIR}/sdl2)

include_directories(
    ${SRC_DIR}
    ${IMGUI_DIR}
    ${GL3W_DIR}
    ${IMGUI_BIND_DIR}
    ${INCLUDE_DIR}
    ${SDL2_DIR}
)

link_directories(
    libs/sdl2/x64
)

link_libraries(sdl2)

# add the library
add_library(imguiWin SHARED
    ${SRC_DIR}/dllmain.cpp
    ${SRC_DIR}/imguiWin.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${GL3W_DIR}/gl3w.c
    ${IMGUI_BIND_DIR}/imgui_impl_opengl3.cpp
    ${IMGUI_BIND_DIR}/imgui_impl_sdl.cpp
)