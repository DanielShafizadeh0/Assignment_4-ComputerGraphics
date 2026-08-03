cmake_minimum_required(VERSION 3.16)

project(Assignment_4-ComputerGraphics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(Assignment_4-ComputerGraphics main.cpp ${HEADERS}
    Torus.h Torus.cpp
    Utils.h
    Utils.cpp
    vertShader.glsl
fragShader.glsl
rustedcopper.jpg
paintedblackmetal.jpg
paintedolivewood.jpg
vertCShader.glsl
fragCShader.glsl
brick1.jpg
cubeMap/xn.jpg cubeMap/xp.jpg cubeMap/yn.jpg cubeMap/yp.jpg cubeMap/zn.jpg cubeMap/zp.jpg
PhongShaders/fragShader.glsl PhongShaders/vertShader.glsl)
include_directories(include /usr/local/include)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_library(SOIL2 soil2-debug)
target_link_libraries(Assignment_4-ComputerGraphics GLEW::GLEW ${OPENGL_LIBRARIES} glm::glm glfw ${SOIL2})


if(WIN32)
    target_link_libraries(Assignment_4-ComputerGraphics)
endif()
if(MSVC)
    add_definitions(/MP /wd4244)   # Parallel build (/MP)
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${src_files})  #Allow to explore source directories as a tree in Visual Studio
endif()
if(UNIX)
    target_include_directories(Assignment_4-ComputerGraphics PUBLIC /usr/local/include /usr/lib64)
    include(GNUInstallDirs)
endif()

#add_dependencies(Assignment_4-ComputerGraphics PUBLIC )

include(GNUInstallDirs)
install(TARGETS Assignment_4-ComputerGraphics
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
