cmake_minimum_required(VERSION 3.15)
project(proj1_cse240a_clion)

set(CMAKE_C_STANDARD 99)

include_directories(src)

add_executable(predictor
        src/main.c
        src/predictor.c)
