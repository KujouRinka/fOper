cmake_minimum_required(VERSION 3.26)
project(file_gen_c99 C)

set(CMAKE_C_STANDARD 99)

include_directories(./common ./config ./action ./file)

file(GLOB_RECURSE FILES ./common/*.c ./config/*.c ./action/*.c ./file/*.c)

add_executable(file_gen_c99 main.c ${FILES} common/global.h)
target_link_libraries(file_gen_c99 yaml)
