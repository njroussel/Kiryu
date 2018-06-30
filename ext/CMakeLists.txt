## Eigen
set(EIGEN_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/eigen/)

set(CompilerFlags 
    EIGEN_INCLUDE_DIR)


## Global
foreach(CompilerFlag ${CompilerFlags})
    set(${CompilerFlag} "${${CompilerFlag}}" PARENT_SCOPE)
endforeach()
