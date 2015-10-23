#     _    ____  _  __
#    / \  / ___|| |/ /  ASK
#   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
#  / ___ \ ___) | . \   -------------------------------------
# /_/   \_\____/|_|\_\  www.codegazoline.it/ask
#                 v0.1
# ===========================================================================
#
# Copyright (C) 2015, The CodeGazoline Team - gargantua AT codegazoline DOT it
# Luca {joshuagame} Stasio - joshuagame AT gmail DOT com
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

# Copyright (C) 2010-2014 KWARC Group <kwarc.info>
#
# This file is part of MathWebSearch.
#
# MathWebSearch is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MathWebSearch is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MathWebSearch.  If not, see <http://www.gnu.org/licenses/>.
#
#
# MICROHTTPD_FOUND - system has MicroHttpd
# MICROHTTPD_INCLUDE_DIRS - the MicroHttpd include directory
# MICROHTTPD_LIBRARIES - Link these to use MicroHttpd
# MICROHTTPD_DEFINITIONS - Compiler switches required for using MicroHttpd

IF (MICROHTTPD_INCLUDE_DIRS AND MICROHTTPD_LIBRARIES)
    SET(MICROHTTPD_FIND_QUIETLY TRUE)
ENDIF (MICROHTTPD_INCLUDE_DIRS AND MICROHTTPD_LIBRARIES)

FIND_PACKAGE (PkgConfig REQUIRED QUIET)
PKG_CHECK_MODULES (PC_MICROHTTPD QUIET libmicrohttpd)

FIND_PATH(MICROHTTPD_INCLUDE_DIRS
        NAMES
        microhttpd.h
        PATH_SUFFIXES
        microhttpd
        HINTS
        ${PC_MICROHTTPD_INCLUDE_DIRS}
        /usr/include
        /usr/local/include
        $ENV{MICROHTTPD}
        $ENV{MICROHTTPD}/include
)

FIND_LIBRARY(MICROHTTPD_LIBRARIES
        NAMES
        microhttpd libmicrohttpd.dll.a libmicrohttd.a
        HINTS
        ${PC_MICROHTTPD_LIBRARY_DIRS}
        /usr/lib
        /usr/local/lib
        $ENV{MICROHTTPD}
        $ENV{MICROHTTPD}/lib
)

SET(MICROHTTPD_DEFINITIONS "")
IF (${PC_MICROHTTPD_FOUND})
    IF (${PC_MICROHTTPD_VERSION} LESS 0.9)
        SET(MICROHTTPD_DEFINITIONS "-DMICROHTTPD_DEPRECATED")
    ENDIF(${PC_MICROHTTPD_VERSION} LESS 0.9)
ENDIF(${PC_MICROHTTPD_FOUND})

# handle the QUIETLY and REQUIRED arguments and set *_FOUND
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MicroHttpd DEFAULT_MSG MICROHTTPD_LIBRARIES MICROHTTPD_INCLUDE_DIRS)

MARK_AS_ADVANCED(MICROHTTPD_INCLUDE_DIRS MICROHTTPD_LIBRARIES MICROHTTPD_DEFINITIONS)