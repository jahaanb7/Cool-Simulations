cmake_minimum_required(VERSION 3.16)
project(ParticleSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(simulation
    main.cpp
)

target_include_directories(simulation PRIVATE library)

target_link_libraries(simulation
    OpenGL::GL
    glfw
    glm::glm
)

add_executable(simulation
    main.cpp
    glad.c
)