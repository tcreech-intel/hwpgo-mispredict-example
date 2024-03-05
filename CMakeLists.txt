cmake_minimum_required(VERSION 3.20.0)
project(spgo_example LANGUAGES C)

#Include the HWPGO helper module after `project()`.
include(${CMAKE_CURRENT_SOURCE_DIR}/HWPGO.cmake)

add_executable(unpredictable unpredictable.c nop.c)
add_executable(predictable   predictable.c   nop.c)

# Name targets to perform PMU sampling on.
# The first argument is an executable target name.
# Also accepted are an IDENTIFIER argument, for organizational purposes, and an
# ARGUMENTS argument, which is a semicolon-delimited list of arguments to the
# executable to be used during collection.
hwpgo_executable(unpredictable IDENTIFIER training1)
hwpgo_executable(predictable   IDENTIFIER training2)

