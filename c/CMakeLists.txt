cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 11)

project(aoc)

# Setup output directories for Visual Studio.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ./bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ./bin)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ./bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ./bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ./bin)
endforeach()

# Include common part.
include_directories(common/include)
file(GLOB_RECURSE common_src "./common/src/*.c")

# Copy each input file, and compile each c file into an executable.
file(GLOB_RECURSE inputs "./**/input/*.txt")
file(GLOB_RECURSE sources "./20*/src/*.c")

foreach(file ${inputs})
    get_filename_component(input_name ${file} NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/bin/${input_name} COPYONLY)
endforeach()

foreach(file ${sources})
    get_filename_component(exec_name ${file} NAME_WE)
    add_executable(${exec_name} ${file} ${common_src})
endforeach()