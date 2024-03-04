cmake_minimum_required(VERSION 3.26)
project(file_gen_c99 C)

set(CMAKE_C_STANDARD 11)

include_directories(./ ./common)

file(GLOB_RECURSE FILES ./cmd/*.c ./common/*.c ./config/*.c ./action/*.c ./file/*.c)

add_link_options(-lyaml -lacl)

add_executable(file_gen_c99 main.c ${FILES})

# test
add_executable(test_config test/config_test.c ${FILES})

add_executable(test_linux_foper_attr test/linux_foper_attr_test.c ${FILES})

add_executable(test_linux_foper_xattr test/linux_foper_xattr_test.c ${FILES})

add_executable(test_linux_foper_acl test/linux_foper_acl_test.c ${FILES})
