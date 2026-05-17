#include "KeyDerivationService.h"
#include <openssl/evp.h>

bool KeyDerivationService::deriveKey(const std::string& password,
                                      const uint8_t* salt,
                                      size_t saltLen,
                                      std::array<uint8_t, KEY_SIZE>& outKey,
                                      int iterations) const {
    return PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.size()),
        salt,
        static_cast<int>(saltLen),
        iterations,
        EVP_sha256(),
        static_cast<int>(KEY_SIZE),
        outKey.data()
    ) == 1;
}