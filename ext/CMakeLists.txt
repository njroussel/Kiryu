## Eigen
set(EIGEN_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/eigen/)

set(CompilerFlags 
    EIGEN_INCLUDE_DIR)


## NanoGUI
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)

# Change NanoGUI Eigen's dependancy to Kiryu's Eigen
set(NANOGUI_EIGEN_INCLUDE_DIR ${EIGEN_INCLUDE_DIR} CACHE BOOL " " FORCE)

add_subdirectory(nanogui)
set_property(TARGET glfw glfw_objects nanogui nanogui-obj PROPERTY FOLDER "dependencies")

set(NANOGUI_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/nanogui/include)

set(CompilerFlags ${CompilerFlags}
    NANOGUI_EXTRA_INCS NANOGUI_EXTRA_DEFS
    NANOGUI_EXTRA_LIBS NANOGUI_INCLUDE_DIR 
    )


## Global
foreach(CompilerFlag ${CompilerFlags})
    set(${CompilerFlag} "${${CompilerFlag}}" PARENT_SCOPE)
endforeach()
