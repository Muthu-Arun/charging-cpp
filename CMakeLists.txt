cmake_minimum_required(VERSION 3.10)
project(charging)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(OpenSSL REQUIRED)
include_directories(backend/ vendor/crow/ vendor/asio/ vendor/jwt/)
add_executable(charging backend/main.cpp backend/db/db.cpp backend/utils/validate.cpp backend/utils/hash.cpp backend/rest/management.cpp)
target_compile_options(charging PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
target_link_libraries(charging sqlite3 sodium OpenSSL::SSL OpenSSL::Crypto)     