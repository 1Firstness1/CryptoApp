#pragma once
#include "ICryptoProvider.h"
#include "Constants.h"
#include <openssl/evp.h>
#include <openssl/err.h>

class OpenSslProvider : public ICryptoProvider {
public:
    OpenSslProvider();
    ~OpenSslProvider() override;
    
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
    bool initCipher(bool encrypt, 
                    const std::array<uint8_t, KEY_SIZE>& key,
                    const std::array<uint8_t, IV_SIZE>& iv);
    void cleanup();
    
    EVP_CIPHER_CTX* m_ctx;
};