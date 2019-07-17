cmake_minimum_required(VERSION 3.0)
project(kitsu_api)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE CXX STANDARD REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(TOP_DIR ${CMAKE_SOURCE_DIR})
set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external/)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# only support linux/g++ for now
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRAPIDJSON_SSE42 -DSPDLOG_COMPILED_LIB -Wall -Wextra -Wno-unused-variable -Wno-long-long -Wno-unused-parameter -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -pedantic ")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb -Og -mavx")
set(CMAKE_CXX_FLAGS_RELEASE "-O1 -DNDEBUG -mavx")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -ggdb -O2 -mavx")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -mavx")

file(GLOB_RECURSE PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB SPDLOG_SOURCE ${EXTERNAL_DIR}/spdlog/src/spdlog.cpp)
file(GLOB HTTP_PARSER_SOURCE ${EXTERNAL_DIR}/restinio/dev/nodejs/http_parser/http_parser.c)
add_executable(kitsu_api ${SPDLOG_SOURCE} ${HTTP_PARSER_SOURCE} ${PROJECT_SOURCES})

add_subdirectory(external/cpr)

include_directories("${EXTERNAL_DIR}/spdlog/include")
include_directories("${EXTERNAL_DIR}/rapidjson/include")
include_directories("${EXTERNAL_DIR}/json/include")
include_directories("${EXTERNAL_DIR}/restinio/dev")
include_directories("${EXTERNAL_DIR}/restinio/dev/asio/include")
include_directories("${EXTERNAL_DIR}/restinio/dev/fmt/include")
include_directories("${EXTERNAL_DIR}/restinio/dev/nodejs/http_parser")
include_directories(${CPR_INCLUDE_DIRS})

find_package(Threads)

target_link_libraries(kitsu_api ${CMAKE_DL_LIBS})
target_link_libraries(kitsu_api ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(kitsu_api ${CPR_LIBRARIES})



