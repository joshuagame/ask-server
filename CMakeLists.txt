#     _    ____  _  __
#    / \  / ___|| |/ /  ASK
#   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
#  / ___ \ ___) | . \   -------------------------------------
# /_/   \_\____/|_|\_\  www.codegazoline.it/ask
#       v1.0.0_alpha-1
# ===========================================================================
#
# Copyright (C) 2015, The CodeGazoline Team - gargantua@codegazoline.it
#
# CMakeLists.txt
# main entry point for the ASK Server
#
# This file is part of the ASK Server.
#
# The ASK Server is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The ASK Server is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Nome - Programma.If not, see <http://www.gnu.org/licenses/>.
#
# ===========================================================================
#
cmake_minimum_required(VERSION 3.0.2)
project(ask_server C)

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_PRERELEASE "alpha-1")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}_${VERSION_PRERELEASE}")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

# Add local CMake module paths
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# compiler flags
#SET(COMMON_FLAGS "-Wall -W -Wextra -Wno-flexible-array-extensions")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -std=c++0x")


# source code
set(SOURCE_FILES askd.c version.h)

# include directories
include(FindMicroHttpd)
INCLUDE_DIRECTORIES(${MICROHTTPD_INCLUDE_DIRS})

# target and Libraries
add_executable(ask_server ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(ask_server ${MICROHTTPD_LIBRARIES})
