#pragma once
#include <string>
#include <array>
#include <cstdint>
#include "CryptoHeader.h"
#include "Constants.h"

class ICryptoProvider {
public:
    virtual ~ICryptoProvider() = default;

    virtual bool encrypt(const std::string& inputPath,
                         const std::string& outputPath,
                         const std::array<uint8_t, KEY_SIZE>& key,
                         const std::array<uint8_t, IV_SIZE>& iv) = 0;

    virtual bool decrypt(const std::string& inputPath,
                         const std::string& outputPath,
                         const std::array<uint8_t, KEY_SIZE>& key,
                         const std::array<uint8_t, IV_SIZE>& iv) = 0;
    
    virtual CryptoHeader::Backend getBackendType() const = 0;
};