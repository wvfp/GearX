####################################################################################
#                                                                                  #
#  Copyright (c) 2014, 2015 - 2016 Axel Menzel <info@rttr.org>                     #
#                                                                                  #
#  This file is part of RTTR (Run Time Type Reflection)                            #
#  License: MIT License                                                            #
#                                                                                  #
#  Permission is hereby granted, free of charge, to any person obtaining           #
#  a copy of this software and associated documentation files (the "Software"),    #
#  to deal in the Software without restriction, including without limitation       #
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,        #
#  and/or sell copies of the Software, and to permit persons to whom the           #
#  Software is furnished to do so, subject to the following conditions:            #
#                                                                                  #
#  The above copyright notice and this permission notice shall be included in      #
#  all copies or substantial portions of the Software.                             #
#                                                                                  #
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      #
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        #
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     #
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          #
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   #
#  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   #
#  SOFTWARE.                                                                       #
#                                                                                  #
####################################################################################

####################################################################################
# Welcome to the CMake build system for RTTR( Run Time Type Reflection).           #
# This is the main file where the general build environment is set-up and the      #
# the build configuration options are initialized.                                 # 
####################################################################################

cmake_minimum_required (VERSION 3.18)
cmake_policy(SET CMP0057 NEW)
project ("rttr")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

if (CMAKE_BUILD_TYPE STREQUAL "")
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

# our little cmake helper functions
include(utility)

# set up option variable for cmake
option(BUILD_STATIC "Build RTTR as static library" FALSE)
option(BUILD_WITH_STATIC_RUNTIME_LIBS "Link against the static runtime libraries" FALSE)
option(BUILD_WITH_RTTI "Enable build with C++ runtime type information for compilation" TRUE)
option(BUILD_BENCHMARKS "Enable this to build the benchmarks" FALSE)
option(USE_PCH "Use precompiled header files for compilation" TRUE)
option(CUSTOM_DOXYGEN_STYLE "Enable this option to use a custom doxygen style for HTML documentation; Otherwise the default will be used" FALSE)
option(BUILD_WEBSITE_DOCU "Enable this option to create the special docu for the website" FALSE)

# one precompiled headers cannot be used for multiple ninja targets
# thats why we have to disable this option, when BUILD_STATIC or 
# BUILD_WITH_STATIC_RUNTIME_LIBS is ON (every target will create the same PCH.pch file)
# to get it working, we need the feature to enable different source properties
# for different targets
if (USE_PCH)
  if (BUILD_STATIC OR BUILD_WITH_STATIC_RUNTIME_LIBS)
    set(USE_PCH FALSE)
  endif()
endif()

include(config)
include(3rd_party_libs)
#include(installer)

# here we add our source code
add_subdirectory(src) 
# and the documentation
#add_subdirectory(doc) 

# here we specify the installation directory
#set(CMAKE_INSTALL_PREFIX ${RTTR_INSTALL_DIR} CACHE PATH  "RTTR install prefix" FORCE)
