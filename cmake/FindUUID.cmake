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

# - Find UUID
# Find the native UUID includes and library
# This module defines
#  UUID_INCLUDE_DIR, where to find jpeglib.h, etc.
#  UUID_LIBRARIES, the libraries needed to use UUID.
#  UUID_FOUND, If false, do not try to use UUID.
# also defined, but not for general use are
#  UUID_LIBRARY, where to find the UUID library.
#
#  Copyright (c) 2006-2009 Mathieu Malaterre <mathieu.malaterre@gmail.com>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# On MacOSX we have:
# $ nm -g /usr/lib/libSystem.dylib | grep uuid_generate
# 000b3aeb T _uuid_generate
# 0003e67e T _uuid_generate_random
# 000b37a1 T _uuid_generate_time
IF(APPLE)
    SET(UUID_LIBRARY_VAR System)
ELSE(APPLE)
    # Linux type:
    SET(UUID_LIBRARY_VAR uuid)
ENDIF(APPLE)

FIND_LIBRARY(UUID_LIBRARY
        NAMES ${UUID_LIBRARY_VAR}
        PATHS /lib /usr/lib /usr/local/lib
)

# Must be *after* the lib itself
SET(CMAKE_FIND_FRAMEWORK_SAVE ${CMAKE_FIND_FRAMEWORK})
SET(CMAKE_FIND_FRAMEWORK NEVER)

FIND_PATH(UUID_INCLUDE_DIR uuid/uuid.h
        /usr/local/include
        /usr/include
)

IF (UUID_LIBRARY AND UUID_INCLUDE_DIR)
    SET(UUID_LIBRARIES ${UUID_LIBRARY})
    SET(UUID_FOUND "YES")
ELSE (UUID_LIBRARY AND UUID_INCLUDE_DIR)
    SET(UUID_FOUND "NO")
ENDIF (UUID_LIBRARY AND UUID_INCLUDE_DIR)


IF (UUID_FOUND)
    IF (NOT UUID_FIND_QUIETLY)
        MESSAGE(STATUS "Found UUID: ${UUID_LIBRARIES}")
    ENDIF (NOT UUID_FIND_QUIETLY)
ELSE (UUID_FOUND)
    IF (UUID_FIND_REQUIRED)
        MESSAGE( "library: ${UUID_LIBRARY}" )
        MESSAGE( "include: ${UUID_INCLUDE_DIR}" )
        MESSAGE(FATAL_ERROR "Could not find UUID library")
    ENDIF (UUID_FIND_REQUIRED)
ENDIF (UUID_FOUND)

# Deprecated declarations.
#SET (NATIVE_UUID_INCLUDE_PATH ${UUID_INCLUDE_DIR} )
#GET_FILENAME_COMPONENT (NATIVE_UUID_LIB_PATH ${UUID_LIBRARY} PATH)

MARK_AS_ADVANCED(
        UUID_LIBRARY
        UUID_INCLUDE_DIR
)
SET(CMAKE_FIND_FRAMEWORK ${CMAKE_FIND_FRAMEWORK_SAVE})
