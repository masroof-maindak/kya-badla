cmake_minimum_required(VERSION 3.14)
project(KyaBadla LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(MOLD_BIN mold)
find_program(LLD_BIN lld)
if(MOLD_BIN)
    set(CMAKE_LINKER_TYPE MOLD)
elseif(LLD_BIN)
    set(CMAKE_LINKER_TYPE LLD)
endif()

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_MakeAvailable(argparse)

find_package(OpenCV REQUIRED)
add_executable(
    kybdl
    "src/main.cpp"
    "src/args.cpp"
    "src/io.cpp"
    "src/img_process.cpp"
    "src/stats.cpp"
)
target_include_directories(kybdl PRIVATE "include/" ${OpenCV_INCLUDE_DIRS})
target_link_libraries(kybdl PRIVATE argparse ${OpenCV_LIBS})
