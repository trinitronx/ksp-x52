# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindKrpcCnano
-----------

Find the kRPC C-nano libraries and headers

IMPORTED targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` target:

``Krpc::KrpcCnano``

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables if found:

``KrpcCnano_INCLUDE_DIRS``
  where to find krpc_cnano.h, etc.
``KrpcCnano_LIBRARIES``
  the libraries to link against to use kRPC C-nano.
``KrpcCnano_VERSION``
  version of the kRPC C-nano library found
``KrpcCnano_FOUND``
  TRUE if found

#]=======================================================================]

# Look for the necessary header
find_path(KrpcCnano_INCLUDE_DIR NAMES krpc_cnano.h)
mark_as_advanced(KrpcCnano_INCLUDE_DIR)

# Look for the necessary library
find_library(KrpcCnano_LIBRARY NAMES libkrpc_cnano krpc_cnano)
mark_as_advanced(KrpcCnano_LIBRARY)

## Version info nowhere to be found in *.h files...
## In case it ever is added, add something like this:
# Extract version information from the header file
# if(KrpcCnano_INCLUDE_DIR)
#     file(STRINGS ${KrpcCnano_INCLUDE_DIR}/krpc_cnano.h _ver_line
#          REGEX "^#define KRPCNANO_VERSION  *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
#          LIMIT_COUNT 1)
#     string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
#            KrpcCnano_VERSION "${_ver_line}")
#     unset(_ver_line)
# endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KrpcCnano
    REQUIRED_VARS KrpcCnano_INCLUDE_DIR KrpcCnano_LIBRARY)
    # VERSION_VAR KrpcCnano_VERSION)

# Create the imported target
if(KrpcCnano_FOUND)
    set(KrpcCnano_INCLUDE_DIRS ${KrpcCnano_INCLUDE_DIR})
    set(KrpcCnano_LIBRARIES ${KrpcCnano_LIBRARY})
    if(NOT TARGET Krpc::KrpcCnano)
        add_library(Krpc::KrpcCnano UNKNOWN IMPORTED)
        set_target_properties(Krpc::KrpcCnano PROPERTIES
            IMPORTED_LOCATION             "${KrpcCnano_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${KrpcCnano_INCLUDE_DIR}")
    endif()
endif()
