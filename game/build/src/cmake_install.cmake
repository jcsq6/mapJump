# Install script for directory: C:/Users/jcsqu/Dropbox/c++Dev/mapJump/game/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/map_jumper")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/dev/msys2/mingw64/bin/objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/jcsqu/Dropbox/c++Dev/mapJump/game/build/src/utils/cmake_install.cmake")
  include("C:/Users/jcsqu/Dropbox/c++Dev/mapJump/game/build/src/physics/cmake_install.cmake")
  include("C:/Users/jcsqu/Dropbox/c++Dev/mapJump/game/build/src/math/cmake_install.cmake")
  include("C:/Users/jcsqu/Dropbox/c++Dev/mapJump/game/build/src/graphics/cmake_install.cmake")
  include("C:/Users/jcsqu/Dropbox/c++Dev/mapJump/game/build/src/game/cmake_install.cmake")

endif()
