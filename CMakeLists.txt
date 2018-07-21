cmake_minimum_required (VERSION 2.8.3)
project(kiryu)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

add_subdirectory(ext)

add_executable(kiryu
    # Header files
    include/kiryu/common.h
    include/kiryu/vector.h
    include/kiryu/ray.h
    include/kiryu/aabb.h

    include/kiryu/sensor.h
    include/kiryu/mesh.h
    include/kiryu/scene.h
    include/kiryu/accel.h
    include/kiryu/bruteAccel.h
    include/kiryu/kdtree.h
    include/kiryu/integrator.h
    include/kiryu/normalIntegrator.h
    include/kiryu/screen.h

    # Source files
    src/sensor.cpp
    src/mesh.cpp
    src/scene.cpp
    src/accel.cpp
    src/bruteAccel.cpp
    src/kdtree.cpp
    src/integrator.cpp
    src/normalIntegrator.cpp
    src/screen.cpp
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
    # TinyObjLoader OBJ mesh loader
    ${TINYOBJLOADER_INCLUDE_DIR}
    )

target_link_libraries(kiryu glfw glad tinyobjloader ${CMAKE_THREAD_LIBS_INIT})
