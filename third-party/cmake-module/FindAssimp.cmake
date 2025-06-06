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

if (NOT TARGET ASSIMP)
    if(WIN32)
        # Set source directory
        set(SOURCE_DIR ${PROJECT_ROOT}/third-party/assimp-5.4.3)
        # Set include directories
        set(INCLUDE_DIRS ${SOURCE_DIR}/include)

        find_library(ASSIMP_LIB
            NAMES assimp-vc143-mt.lib
            PATHS ${SOURCE_DIR}/libs
            NO_DEFAULT_PATH)

        find_file(ASSIMP_DLL
            NAMES assimp-vc143-mt.dll
            PATHS ${SOURCE_DIR}/bins
            NO_DEFAULT_PATH)

        add_library(ASSIMP SHARED IMPORTED)
        set_target_properties(ASSIMP PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}"
            IMPORTED_IMPLIB_DEBUG ${ASSIMP_LIB}
            IMPORTED_IMPLIB_RELEASE ${ASSIMP_LIB}
            IMPORTED_LOCATION_DEBUG ${ASSIMP_DLL}
            IMPORTED_LOCATION_RELEASE ${ASSIMP_DLL}
        )
    endif()
endif()
