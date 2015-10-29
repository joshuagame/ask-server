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

# - Try to find openssl include dirs and libraries
#
# Usage of this module as follows:
#
#     find_package(OpenSSL)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  OpenSSL_ROOT_DIR          Set this variable to the root installation of
#                            openssl if the module has problems finding the
#                            proper installation path.
#
# Variables defined by this module:
#
#  OPENSSL_FOUND             System has openssl, include and library dirs found
#  OpenSSL_INCLUDE_DIR       The openssl include directories.
#  OpenSSL_LIBRARIES         The openssl libraries.
#  OpenSSL_CYRPTO_LIBRARY    The openssl crypto library.
#  OpenSSL_SSL_LIBRARY       The openssl ssl library.

find_path(OpenSSL_ROOT_DIR
        NAMES include/openssl/ssl.h
)

find_path(OpenSSL_INCLUDE_DIR
        NAMES openssl/ssl.h
        HINTS ${OpenSSL_ROOT_DIR}/include
)

find_library(OpenSSL_SSL_LIBRARY
        NAMES ssl ssleay32 ssleay32MD
        HINTS ${OpenSSL_ROOT_DIR}/lib
)

find_library(OpenSSL_CRYPTO_LIBRARY
        NAMES crypto
        HINTS ${OpenSSL_ROOT_DIR}/lib
)

set(OpenSSL_LIBRARIES ${OpenSSL_SSL_LIBRARY} ${OpenSSL_CRYPTO_LIBRARY}
        CACHE STRING "OpenSSL SSL and crypto libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenSSL DEFAULT_MSG
        OpenSSL_LIBRARIES
        OpenSSL_INCLUDE_DIR
)

mark_as_advanced(
        OpenSSL_ROOT_DIR
        OpenSSL_INCLUDE_DIR
        OpenSSL_LIBRARIES
        OpenSSL_CRYPTO_LIBRARY
        OpenSSL_SSL_LIBRARY
)