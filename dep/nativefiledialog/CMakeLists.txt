cmake_minimum_required(VERSION 3.8)

if (WIN32)
	add_library(nfd STATIC nfd_common.c nfd_win.cpp)
elseif (APPLE)
	add_library(nfd STATIC nfd_common.c nfd_cocoa.m)
elseif (UNIX)
	add_library(nfd STATIC nfd_common.c nfd_gtk.c)
endif()

target_include_directories(nfd PUBLIC include)