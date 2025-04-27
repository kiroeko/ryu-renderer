# Marcos
# Get parent directory
macro(get_parent_dierectory DIR_PATH DEPTH)
    set(GPD_CUR_DIR ${DIR_PATH})
    foreach(var RANGE 1 ${DEPTH})
        cmake_path(GET GPD_CUR_DIR PARENT_PATH GPD_CUR_DIR)
    endforeach()
    set(GPD_RESULT ${GPD_CUR_DIR})
endmacro()

# Set required variables
get_parent_dierectory(${CMAKE_CURRENT_LIST_DIR} 2)
set(PROJECT_ROOT ${GPD_RESULT})
unset(GPD_RESULT)

if (NOT TARGET Glad)
    if(WIN32)
        # Set source directory
        set(SOURCE_DIR ${PROJECT_ROOT}/third-party/glad-2)
        # Set include directories
        set(INCLUDE_DIRS ${SOURCE_DIR}/include)
        # Set source
        set(GLAD_SRC_LIST
            ${SOURCE_DIR}/src/glad.cpp
            CACHE INTERNAL "GLFW Source list")

        add_library(Glad INTERFACE IMPORTED)
        set_target_properties(Glad PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}"
        )
    endif()
endif()
