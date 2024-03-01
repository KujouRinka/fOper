cmake_minimum_required(VERSION 3.26)
project(file_gen_c99 C)

set(CMAKE_C_STANDARD 11)

include_directories(./ ./common)

file(GLOB_RECURSE FILES ./cmd/*.c ./common/*.c ./config/*.c ./action/*.c ./file/*.c)

add_executable(file_gen_c99 main.c ${FILES})
target_link_libraries(file_gen_c99 yaml)

# test
add_executable(test_config test/config_test.c ${FILES})
target_link_libraries(test_config yaml)

add_executable(test_linux_foper test/linux_foper_test.c ${FILES})
target_link_libraries(test_linux_foper yaml)
