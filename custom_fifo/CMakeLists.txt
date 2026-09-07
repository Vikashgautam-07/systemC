# CMakeLists.txt for CustomFifo SystemC project
#
# Usage:
#   mkdir build && cd build
#   cmake .. -DSYSTEMC_HOME=/usr/local/systemc
#   make
#   ./custom_fifo
#
# If SYSTEMC_HOME isn't passed, it defaults to /usr/local/systemc — change the
# default below if your install lives elsewhere, or always pass -DSYSTEMC_HOME=...

cmake_minimum_required(VERSION 3.10)
project(custom_fifo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow override from the command line: cmake .. -DSYSTEMC_HOME=/path
if(NOT DEFINED SYSTEMC_HOME)
    set(SYSTEMC_HOME "/usr/local/systemc" CACHE PATH "Path to SystemC install")
endif()

add_executable(custom_fifo custom_fifo.cc)

target_include_directories(custom_fifo PRIVATE ${SYSTEMC_HOME}/include)
target_link_directories(custom_fifo PRIVATE ${SYSTEMC_HOME}/lib-linux64)
target_link_libraries(custom_fifo PRIVATE systemc pthread)

target_compile_options(custom_fifo PRIVATE -Wall -Wextra -g)

# So the binary finds libsystemc.so at runtime without manually setting
# LD_LIBRARY_PATH — bakes the library path directly into the executable.
set_target_properties(custom_fifo PROPERTIES
    BUILD_RPATH "${SYSTEMC_HOME}/lib-linux64"
)