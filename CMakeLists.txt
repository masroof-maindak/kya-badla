cmake_minimum_required(VERSION 3.14)
project(ChangeDetector)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Linker
find_program(MOLD_BIN mold)
find_program(LLD_BIN lld)
if(MOLD_BIN)
	set(CMAKE_LINKER_TYPE MOLD)
elseif(LLD_BIN)
	set(CMAKE_LINKER_TYPE LLD)
endif()

# Argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

# Cmake
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Executable & Linking
add_executable(chng "main.cpp")
target_link_libraries(chng PRIVATE argparse ${OpenCV_LIBS})
target_link_libraries(chng )
