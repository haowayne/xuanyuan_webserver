cmake_minimum_required(VERSION 3.19)
project(xuanyuan C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS -pthread)

add_executable(xuanyuan
        day8/main.c ../cJSON/cJSON.c)
