#pragma once
#include <string>
#include <array>
#include <cstdint>
#include "Constants.h"

class KeyDerivationService {
public:
    KeyDerivationService() = default;
    
    bool deriveKey(const std::string& password,
                   const uint8_t* salt,
                   size_t saltLen,
                   std::array<uint8_t, KEY_SIZE>& outKey,
                   int iterations = PBKDF2_ITERATIONS) const;
};