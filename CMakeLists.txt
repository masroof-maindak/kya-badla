cmake_minimum_required(VERSION 3.10)
project(opencv-cmake-template)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

find_program(MOLD_BIN mold)
find_program(LLD_BIN lld)
if(MOLD_BIN)
	set(CMAKE_LINKER_TYPE MOLD)
elseif(LLD_BIN)
	set(CMAKE_LINKER_TYPE LLD)
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(demo "main.cpp")
target_link_libraries(demo ${OpenCV_LIBS})
