cmake_minimum_required(VERSION 3.30)
project(crypto_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции для тестирования
option(BUILD_TESTS "Build unit tests" ON)

find_package(OpenSSL REQUIRED)

# Основная библиотека
add_library(crypto_lib STATIC
        src/Options.cpp
        src/CryptoApp.cpp
        src/CryptoHeader.cpp
        src/KeyDerivationService.cpp
        src/FileIOService.cpp
        src/SimpleAesProvider.cpp
        src/OpenSslProvider.cpp
)

target_include_directories(crypto_lib PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(crypto_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(crypto_lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Основное приложение
add_executable(crypto_app src/main.cpp)
target_link_libraries(crypto_app PRIVATE crypto_lib)

# Тесты
if(BUILD_TESTS)
    enable_testing()

    # Скачиваем GoogleTest
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    FetchContent_MakeAvailable(googletest)

    # Директория с тестами
    add_subdirectory(tests)
endif()