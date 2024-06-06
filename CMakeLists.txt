cmake_minimum_required(VERSION 2.8.4...3.29)
project(kRPC-x52)

set(VERSION_NUMBER 0.0.1)
set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (NOT DEFINED CMAKE_MODULE_PATH)
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
endif()

file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_executable(ksp-x52  ${SRC})

find_package(PkgConfig REQUIRED)
pkg_check_modules(X52 REQUIRED libx52)
find_package(KrpcCnano REQUIRED)
target_link_libraries(ksp-x52 PUBLIC "${X52_LIBRARIES}")
target_link_libraries(ksp-x52 PUBLIC "${KrpcCnano_LIBRARIES}")

target_include_directories(ksp-x52 PUBLIC ${X52_INCLUDE_DIRS})
target_include_directories(ksp-x52 PUBLIC ${KrpcCnano_INCLUDE_DIRS})
target_compile_options(ksp-x52 PUBLIC ${X52_CFLAGS_OTHER})
target_compile_options(ksp-x52 PUBLIC ${KrpcCnano_CFLAGS_OTHER})

# find_package(Protobuf 3.2 REQUIRED)

set_target_properties(
        ksp-x52 PROPERTIES  VERSION   ${VERSION_NUMBER})


install(TARGETS ksp-x52
        RUNTIME COMPONENT Runtime
        LIBRARY COMPONENT Library
        ARCHIVE COMPONENT Development)

# install(DIRECTORY include DESTINATION . FILES_MATCHING PATTERN "*.h")
