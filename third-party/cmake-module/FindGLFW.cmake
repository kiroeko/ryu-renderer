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

if (NOT TARGET GLFW)
    if(WIN32)
        # Set source directory
        set(SOURCE_DIR ${PROJECT_ROOT}/third-party/glfw-3.4)
        # Set include directories
        set(INCLUDE_DIRS ${SOURCE_DIR}/include)
        # Set definitions
        set(GLFW_DEFINITIONS
            "GLFW_DLL"
            "GLFW_INCLUDE_NONE"
            CACHE INTERNAL "GLFW Definitions")

        find_library(GLFW_LIB
            NAMES glfw3dll.lib
            PATHS ${SOURCE_DIR}/libs
            NO_DEFAULT_PATH)

        find_file(GLFW_DLL
            NAMES glfw3.dll
            PATHS ${SOURCE_DIR}/bins
            NO_DEFAULT_PATH)

        add_library(GLFW SHARED IMPORTED)
        set_target_properties(GLFW PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}"
            IMPORTED_IMPLIB_DEBUG ${GLFW_LIB}
            IMPORTED_IMPLIB_RELEASE ${GLFW_LIB}
            IMPORTED_LOCATION_DEBUG ${GLFW_DLL}
            IMPORTED_LOCATION_RELEASE ${GLFW_DLL}
        )
    endif()
endif()
