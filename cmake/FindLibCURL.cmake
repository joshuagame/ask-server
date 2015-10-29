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
find_path(LibCURL_ROOT_DIR
        NAMES include/curl/curl.h
)

find_library(LibCURL_LIBRARIES
        NAMES curl
        HINTS ${LibCURL_ROOT_DIR}/lib
)

find_path(LibCURL_INCLUDE_DIR
        NAMES curl/curl.h
        HINTS ${LibCURL_ROOT_DIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibCURL DEFAULT_MSG
        LibCURL_LIBRARIES
        LibCURL_INCLUDE_DIR
)

mark_as_advanced(
        LibCURL_ROOT_DIR
        LibCURL_LIBRARIES
        LibCURL_INCLUDE_DIR
)