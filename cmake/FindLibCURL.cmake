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