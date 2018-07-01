cmake_minimum_required (VERSION 2.8.3)
project(kiryu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ext)

add_executable(kiryu
    # Source files
    src/main.cpp
    )

target_include_directories(kiryu PRIVATE
    # Kiryu include files
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # Eigen linear algebra library
    ${EIGEN_INCLUDE_DIR}
    # GLFW OpenGL/Vulkan graphcis library framework
    ${GLFW_INCLUDE_DIR}
    # GLAD OpenGL function loader
    ${GLAD_INCLUDE_DIR}
    )

target_link_libraries(kiryu glfw glad)
