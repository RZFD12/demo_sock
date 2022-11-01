cmake_minimum_required(VERSION 3.5)

project(demo_sock LANGUAGES C)
set(CMAKE_C_STANDARD 17)

add_executable(demo_sock main.c Tlv.h Tlv.c Test.c Test.h)
