# Marcos
# Group sources
macro(source_group_by_dir SOURCE_FILES)
    set(SGBD_CUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(SGBD_FILE ${${SOURCE_FILES}})
        string(REGEX REPLACE ${SGBD_CUR_DIR}/\(.*\) \\1 SGBD_FILE_PATH ${SGBD_FILE})
        string(REGEX REPLACE "\(.*\)/.*" \\1 SGBD_GROUP_NAME ${SGBD_FILE_PATH})
        string(COMPARE EQUAL ${SGBD_FILE_PATH} ${SGBD_GROUP_NAME} SGBD_NOGROUP)
        string(REPLACE "/" "\\" SGBD_GROUP_NAME ${SGBD_GROUP_NAME})
        if(SGBD_NOGROUP)
            set(SGBD_GROUP_NAME "\\")
        endif()
        source_group(${SGBD_GROUP_NAME} FILES ${SGBD_FILE})
    endforeach()
    unset(SGBD_CUR_DIR)
    unset(SGBD_FILE)
    unset(SGBD_FILE_PATH)
    unset(SGBD_GROUP_NAME)
    unset(SGBD_NOGROUP)
endmacro()

# Make directory
macro(make_directory DIR)
    if(EXISTS ${DIR})
        file(REMOVE_RECURSE ${DIR})
    endif()
    file(MAKE_DIRECTORY ${DIR})
endmacro()

# Cmake version requirements define
cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

# Define project
project(ryu-renderer
        LANGUAGES C CXX)

# Check environment
if (NOT WIN32 OR NOT MSVC)
    message(FATAL_ERROR "Fatal Error: This project requires Visual Studio 2022 and Windows environment to build.")
endif()
if (NOT "${MSVC_TOOLSET_VERSION}" STREQUAL "143")
    message(FATAL_ERROR "Fatal Error: This project requires Visual Studio 2022")
endif()

# Set language version
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check basic project properties
if (NOT CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM x64)
endif()
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# Set output directory
string(TOLOWER ${CMAKE_SYSTEM_NAME} CMAKE_SYSTEM_NAME_STR)
string(TOLOWER ${CMAKE_GENERATOR_PLATFORM} CMAKE_GENERATOR_PLATFORM_STR)
set(OUTPUT_BIN_DIR_DEBUG ${PROJECT_SOURCE_DIR}/output/${CMAKE_SYSTEM_NAME_STR}/${CMAKE_GENERATOR_PLATFORM_STR}/debug/bin)
make_directory(${OUTPUT_BIN_DIR_DEBUG})
set(OUTPUT_BIN_DIR_RELEASE ${PROJECT_SOURCE_DIR}/output/${CMAKE_SYSTEM_NAME_STR}/${CMAKE_GENERATOR_PLATFORM_STR}/release/bin)
make_directory(${OUTPUT_BIN_DIR_RELEASE})

# Set runtime resources
set(${PROJECT_NAME}_RES_DIRS "")
list(APPEND ${PROJECT_NAME}_RES_DIRS
    ${PROJECT_SOURCE_DIR}/res)

# Set vs startup project
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Create executable
add_executable(${PROJECT_NAME})

# Find related libraries
list(INSERT CMAKE_MODULE_PATH 0 ${PROJECT_SOURCE_DIR}/third-party/cmake-module)

find_package(Boost REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Glad REQUIRED)
find_package(GLFW REQUIRED)
find_package(STB REQUIRED)
find_package(GLM REQUIRED)
find_package(ASSIMP REQUIRED)

set(Boost_USE_STATIC_LIBS ON)

# Set definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${GLFW_DEFINITIONS})

# Set complie options
target_compile_options(${PROJECT_NAME} PRIVATE
    -D_UNICODE -DUNICODE "/Zc:__cplusplus"
)

# Set properties
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/output/${CMAKE_SYSTEM_NAME_STR}/${CMAKE_GENERATOR_PLATFORM_STR}/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/bin
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    CXX_VISIBILITY_PRESET hidden
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_BIN_DIR_DEBUG}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_BIN_DIR_RELEASE}
)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
                           ${PROJECT_SOURCE_DIR}/include)

# Collect all source files
file(GLOB_RECURSE SRC_LIST
    ${PROJECT_SOURCE_DIR}/include/*.h
    ${PROJECT_SOURCE_DIR}/include/*.hpp
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(APPEND SRC_LIST ${GLAD_SRC_LIST})
list(APPEND SRC_LIST ${STB_SRC_LIST})
source_group_by_dir(SRC_LIST)

# Set sources
target_sources(${PROJECT_NAME} PRIVATE
               ${SRC_LIST})

# Set linked libs
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    Glad
    GLFW
    STB
    GLM
    Boost::boost
    ASSIMP
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
        copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E
        copy_directory_if_different ${${PROJECT_NAME}_RES_DIRS} $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
        COMMAND_EXPAND_LISTS
)