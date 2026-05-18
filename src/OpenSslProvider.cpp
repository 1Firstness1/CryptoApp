#include "OpenSslProvider.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

OpenSslProvider::OpenSslProvider()
    : m_ctx(nullptr) {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

OpenSslProvider::~OpenSslProvider() {
    cleanup();
}

bool OpenSslProvider::initCipher(bool encrypt,
                                   const std::array<uint8_t, KEY_SIZE>& key,
                                   const std::array<uint8_t, IV_SIZE>& iv) {
    cleanup();

    m_ctx = EVP_CIPHER_CTX_new();
    if (!m_ctx) return false;

    if (encrypt) {
        return EVP_EncryptInit_ex(m_ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) == 1;
    } else {
        return EVP_DecryptInit_ex(m_ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) == 1;
    }
}

void OpenSslProvider::cleanup() {
    if (m_ctx) {
        EVP_CIPHER_CTX_free(m_ctx);
        m_ctx = nullptr;
    }
}

CryptoHeader::Backend OpenSslProvider::getBackendType() const {
    return CryptoHeader::Backend::OPENSSL;
}

bool OpenSslProvider::encrypt(const std::string& inputPath,
                               const std::string& outputPath,
                               const std::array<uint8_t, KEY_SIZE>& key,
                               const std::array<uint8_t, IV_SIZE>& iv) {
    if (!initCipher(true, key, iv)) {
        std::cerr << "OpenSslProvider: encrypt init failed\n";
        return false;
    }

    std::ifstream in(inputPath, std::ios::binary);
    std::ofstream out(outputPath, std::ios::binary | std::ios::trunc);

    if (!in || !out) {
        std::cerr << "OpenSslProvider: cannot open files\n";
        return false;
    }

    std::vector<uint8_t> inBuf(CHUNK_SIZE);
    std::vector<uint8_t> outBuf(CHUNK_SIZE + EVP_MAX_BLOCK_LENGTH);
    size_t totalProcessed = 0;

    while (in) {
        in.read(reinterpret_cast<char*>(inBuf.data()), CHUNK_SIZE);
        size_t bytesRead = static_cast<size_t>(in.gcount());

        if (bytesRead == 0) break;

        int outLen = 0;
        if (EVP_EncryptUpdate(m_ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(bytesRead)) != 1) {
            std::cerr << "OpenSslProvider: encrypt update failed\n";
            return false;
        }

        out.write(reinterpret_cast<char*>(outBuf.data()), outLen);
        if (!out) {
            std::cerr << "OpenSslProvider: write failed\n";
            return false;
        }
        totalProcessed += bytesRead;

        if (totalProcessed % (CHUNK_SIZE * 10) == 0 && totalProcessed > 0) {
            std::cout << "\rEncrypting... " << totalProcessed / 1024 << " KB" << std::flush;
        }
    }

    int finalLen = 0;
    if (EVP_EncryptFinal_ex(m_ctx, outBuf.data(), &finalLen) != 1) {
        std::cerr << "OpenSslProvider: encrypt final failed\n";
        return false;
    }

    if (finalLen > 0) {
        out.write(reinterpret_cast<char*>(outBuf.data()), finalLen);
    }

    std::cout << "\rEncrypting... Done (" << totalProcessed / 1024 << " KB)    \n";

    return true;
}

bool OpenSslProvider::decrypt(const std::string& inputPath,
                               const std::string& outputPath,
                               const std::array<uint8_t, KEY_SIZE>& key,
                               const std::array<uint8_t, IV_SIZE>& iv) {
    if (!initCipher(false, key, iv)) {
        std::cerr << "OpenSslProvider: decrypt init failed\n";
        return false;
    }

    std::ifstream in(inputPath, std::ios::binary);
    std::ofstream out(outputPath, std::ios::binary);

    if (!in || !out) {
        std::cerr << "OpenSslProvider: cannot open files\n";
        return false;
    }

    std::vector<uint8_t> inBuf(CHUNK_SIZE);
    std::vector<uint8_t> outBuf(CHUNK_SIZE + EVP_MAX_BLOCK_LENGTH);
    size_t totalProcessed = 0;

    while (in) {
        in.read(reinterpret_cast<char*>(inBuf.data()), CHUNK_SIZE);
        size_t bytesRead = static_cast<size_t>(in.gcount());

        if (bytesRead == 0) break;

        int outLen = 0;
        if (EVP_DecryptUpdate(m_ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(bytesRead)) != 1) {
            std::cerr << "OpenSslProvider: decrypt update failed\n";
            return false;
        }

        out.write(reinterpret_cast<char*>(outBuf.data()), outLen);
        totalProcessed += bytesRead;

        if (totalProcessed % (CHUNK_SIZE * 10) == 0) {
            std::cout << "\rDecrypting... " << totalProcessed / 1024 << " KB" << std::flush;
        }
    }

    int finalLen = 0;
    if (EVP_DecryptFinal_ex(m_ctx, outBuf.data(), &finalLen) != 1) {
        std::cerr << "OpenSslProvider: decrypt failed - wrong password or corrupted file\n";
        return false;
    }

    out.write(reinterpret_cast<char*>(outBuf.data()), finalLen);
    std::cout << "\rDecrypting... Done (" << totalProcessed / 1024 << " KB)\n";
    
    return true;
}