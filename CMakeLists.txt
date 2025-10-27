cmake_minimum_required(VERSION 3.10)
project(charging)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)

# Common source files and include directories
set(COMMON_SOURCES
    backend/db/db.cpp
    backend/utils/validate.cpp
    backend/utils/hash.cpp
    backend/rest/management.cpp
    backend/rest/charging.cpp
    backend/utils/cache.cpp
)

set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/crow
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/asio
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/jwt
)

# Target: charging
add_executable(charging backend/main.cpp ${COMMON_SOURCES})
target_include_directories(charging PRIVATE ${COMMON_INCLUDE_DIRS})
target_compile_options(charging PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(charging PRIVATE sqlite3 sodium OpenSSL::SSL OpenSSL::Crypto)

# Target: charging_debug
add_executable(charging_debug backend/test/embd.cpp ${COMMON_SOURCES})
target_include_directories(charging_debug PRIVATE ${COMMON_INCLUDE_DIRS})
target_compile_options(charging_debug PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(charging_debug PRIVATE sqlite3 sodium OpenSSL::SSL OpenSSL::Crypto)