cmake_minimum_required(VERSION 4.0)

project(basic C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(SDL2 REQUIRED)
find_package(triangle REQUIRED)
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp")
add_library(glad STATIC
glad/src/glad.c
)
target_include_directories(glad PUBLIC glad/include)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
PRIVATE
${SDL2_INCLUDE_DIRS}
glad/include
${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME}
PRIVATE
glad
${SDL2_LIBRARIES}
OpenGL::GL
)
