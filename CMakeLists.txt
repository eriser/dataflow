cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
if(CMAKE_MAJOR_VERSION EQUAL 3)
    cmake_policy(SET CMP0042 NEW) # suppresses warning on OS X
endif()

project(dataflow)

# Add local modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Check for C++11 on GCC and Clang

if(CMAKE_COMPILER_IS_GNUCXX OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W1 /wd4251 /wd4275 /DNOMINMAX")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Setup output binary path
set(DATAFLOW_OUTPUT_DIR "${dataflow_BINARY_DIR}/bin")
file(MAKE_DIRECTORY ${DATAFLOW_OUTPUT_DIR})
set(CMAKE_INSTALL_RPATH ${DATAFLOW_OUTPUT_DIR})

add_subdirectory(basicmath)
add_subdirectory(io_module)
add_subdirectory(dstring)
add_subdirectory(ppm)
add_subdirectory(interpreter)
add_subdirectory(gui)

source_group("Headers" "\\.h$")
source_group("Sources" "\\.c(pp)?$")
