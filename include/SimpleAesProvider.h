#pragma once
#include "ICryptoProvider.h"
#include "Constants.h"

class SimpleAesProvider : public ICryptoProvider {
public:
    SimpleAesProvider() = default;

    bool encrypt(const std::string& inputPath,
                 const std::string& outputPath,
                 const std::array<uint8_t, KEY_SIZE>& key,
                 const std::array<uint8_t, IV_SIZE>& iv) override;

    bool decrypt(const std::string& inputPath,
                 const std::string& outputPath,
                 const std::array<uint8_t, KEY_SIZE>& key,
                 const std::array<uint8_t, IV_SIZE>& iv) override;

    CryptoHeader::Backend getBackendType() const override;

private:
    void processBlock(uint8_t* data, size_t size,
                      const std::array<uint8_t, KEY_SIZE>& key,
                      const std::array<uint8_t, IV_SIZE>& iv,
                      bool encrypt);
};