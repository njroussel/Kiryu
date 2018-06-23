cmake_minimum_required (VERSION 2.8.3)
project(kiryu)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ext)

include_directories(
    # Kiryu include files
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # Eigen linear algebra library
    ${EIGEN_INCLUDE_DIR}
    # NanoGUI user interface library
    ${NANOGUI_INCLUDE_DIR}
    ${NANOGUI_EXTRA_INCS}
    )

add_executable(kiryu
    # Source files
    src/main.cpp
    )

add_definitions(${NANOGUI_EXTRA_DEFS})

target_link_libraries(kiryu nanogui ${NANOGUI_EXTRA_LIBS})
