#pragma once
#include <cstddef>

constexpr size_t SALT_SIZE = 16;
constexpr size_t IV_SIZE = 16;
constexpr size_t KEY_SIZE = 32;
constexpr size_t MAGIC_SIZE = 5;
constexpr size_t CHUNK_SIZE = 64 * 1024;
constexpr int PBKDF2_ITERATIONS = 200000;