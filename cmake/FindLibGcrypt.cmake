#     _    ____  _  __
#    / \  / ___|| |/ /  ASK
#   / _ \ \___ \| ' /   Authentication Sessions Keeper Server
#  / ___ \ ___) | . \   -------------------------------------
# /_/   \_\____/|_|\_\  www.codegazoline.it/ask
#                 v0.1
# ===============================================================================
# Copyright (C) 2015, Luca Stasio - joshuagame@gmail.com //The CodeGazoline Team/
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

FIND_PROGRAM(LIBGCRYPTCONFIG_EXECUTABLE NAMES libgcrypt-config)

#reset variables
set(LIBGCRYPT_LIBRARIES)
set(LIBGCRYPT_CFLAGS)

# if libgcrypt-config has been found
IF(LIBGCRYPTCONFIG_EXECUTABLE)

    EXEC_PROGRAM(${LIBGCRYPTCONFIG_EXECUTABLE} ARGS --libs RETURN_VALUE _return_VALUE OUTPUT_VARIABLE LIBGCRYPT_LIBRARIES)

    EXEC_PROGRAM(${LIBGCRYPTCONFIG_EXECUTABLE} ARGS --cflags RETURN_VALUE _return_VALUE OUTPUT_VARIABLE LIBGCRYPT_CFLAGS)

    IF(${LIBGCRYPT_CFLAGS} MATCHES "\n")
        SET(LIBGCRYPT_CFLAGS " ")
    ENDIF(${LIBGCRYPT_CFLAGS} MATCHES "\n")

    IF(LIBGCRYPT_LIBRARIES AND LIBGCRYPT_CFLAGS)
        SET(LIBGCRYPT_FOUND TRUE)
    ENDIF(LIBGCRYPT_LIBRARIES AND LIBGCRYPT_CFLAGS)

ENDIF(LIBGCRYPTCONFIG_EXECUTABLE)

if (LIBGCRYPT_FOUND)
    if (NOT LibGcrypt_FIND_QUIETLY)
        message(STATUS "Found libgcrypt: ${LIBGCRYPT_LIBRARIES}")
    endif (NOT LibGcrypt_FIND_QUIETLY)
else (LIBGCRYPT_FOUND)
    if (LibGcrypt_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find libgcrypt libraries")
    endif (LibGcrypt_FIND_REQUIRED)
endif (LIBGCRYPT_FOUND)

MARK_AS_ADVANCED(LIBGCRYPT_CFLAGS LIBGCRYPT_LIBRARIES)