cmake_minimum_required(VERSION 3.8)

add_library(tinyfiledialogs STATIC tinyfiledialogs.c)

target_include_directories(tinyfiledialogs PUBLIC .)