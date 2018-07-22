## Kiryu
set(BUILD_SHARED_LIBS ON CACHE BOOL " " FORCE)


## Eigen
set(EIGEN_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/eigen)

set(CompilerFlags
    EIGEN_INCLUDE_DIR)


## GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL " " FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/glfw")

set(GLFW_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)

set(CompilerFlags ${CompilerFlags}
    GLFW_INCLUDE_DIR)


## GLAD
set(GLAD_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

set(CompilerFlags ${CompilerFlags}
    GLAD_INCLUDE_DIR)

add_library(glad SHARED "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c")
target_include_directories(glad PRIVATE ${GLAD_INCLUDE_DIR})


## TinyObjLoader
set(TINYOBJLOADER_COMPILATION_SHARED ON CACHE BOOL " " FORCE)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader")

set(TINYOBJLOADER_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader)

set(CompilerFlags ${CompilerFlags}
    TINYOBJLOADER_INCLUDE_DIR)

## PCG
set(PCG_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/pcg-cpp/include)

set(CompilerFlags ${CompilerFlags}
    PCG_INCLUDE_DIR)

## Kiryu
foreach(CompilerFlag ${CompilerFlags})
    set(${CompilerFlag} "${${CompilerFlag}}" PARENT_SCOPE)
endforeach()
