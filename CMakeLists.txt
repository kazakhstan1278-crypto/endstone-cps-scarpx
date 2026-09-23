cmake_minimum_required(VERSION 3.15)
project(cpsguard VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Endstone 0.11.12 C++ SDK source tree.
# Preferred: pass -DENDSTONE_SOURCE_DIR=/path/to/endstone-main
if(DEFINED ENDSTONE_SOURCE_DIR)
    add_subdirectory("${ENDSTONE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/endstone")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../endstone-main/CMakeLists.txt")
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../endstone-main" "${CMAKE_BINARY_DIR}/endstone")
else()
    include(FetchContent)
    FetchContent_Declare(
        endstone
        GIT_REPOSITORY https://github.com/EndstoneMC/endstone.git
        GIT_TAG v0.11.12
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(endstone)
endif()

endstone_add_plugin(cpsguard plugin.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

if(UNIX AND NOT APPLE)
    target_compile_options(${PROJECT_NAME} PRIVATE -stdlib=libc++)
    target_link_options(${PROJECT_NAME} PRIVATE -stdlib=libc++)
endif()
