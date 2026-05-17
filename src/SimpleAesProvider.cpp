#include "SimpleAesProvider.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

CryptoHeader::Backend SimpleAesProvider::getBackendType() const {
    return CryptoHeader::Backend::SIMPLE;
}

void SimpleAesProvider::processBlock(uint8_t* data, size_t size,
                                      const std::array<uint8_t, KEY_SIZE>& key,
                                      const std::array<uint8_t, IV_SIZE>& iv,
                                      bool encrypt) {
    // Простая XOR-реализация для демонстрации
    for (size_t i = 0; i < size; ++i) {
        data[i] ^= key[i % KEY_SIZE];
        data[i] ^= iv[i % IV_SIZE];
        if (encrypt) {
            data[i] = ~data[i];
        }
    }
}

bool SimpleAesProvider::encrypt(const std::string& inputPath,
                                 const std::string& outputPath,
                                 const std::array<uint8_t, KEY_SIZE>& key,
                                 const std::array<uint8_t, IV_SIZE>& iv) {
    std::ifstream in(inputPath, std::ios::binary);
    std::ofstream out(outputPath, std::ios::binary);

    if (!in || !out) {
        std::cerr << "SimpleAesProvider: cannot open files\n";
        return false;
    }

    std::vector<uint8_t> buffer(CHUNK_SIZE);
    size_t totalProcessed = 0;

    while (in) {
        in.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
        size_t bytesRead = static_cast<size_t>(in.gcount());

        if (bytesRead > 0) {
            processBlock(buffer.data(), bytesRead, key, iv, true);
            out.write(reinterpret_cast<char*>(buffer.data()), bytesRead);
            totalProcessed += bytesRead;

            if (totalProcessed % (CHUNK_SIZE * 10) == 0) {
                std::cout << "\rEncrypting... " << totalProcessed / 1024 << " KB" << std::flush;
            }
        }
    }

    std::cout << "\rEncrypting... Done (" << totalProcessed / 1024 << " KB)\n";
    return true;
}

bool SimpleAesProvider::decrypt(const std::string& inputPath,
                                 const std::string& outputPath,
                                 const std::array<uint8_t, KEY_SIZE>& key,
                                 const std::array<uint8_t, IV_SIZE>& iv) {
    std::ifstream in(inputPath, std::ios::binary);
    std::ofstream out(outputPath, std::ios::binary);

    if (!in || !out) {
        std::cerr << "SimpleAesProvider: cannot open files\n";
        return false;
    }

    std::vector<uint8_t> buffer(CHUNK_SIZE);
    size_t totalProcessed = 0;

    while (in) {
        in.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
        size_t bytesRead = static_cast<size_t>(in.gcount());

        if (bytesRead > 0) {
            processBlock(buffer.data(), bytesRead, key, iv, false);
            out.write(reinterpret_cast<char*>(buffer.data()), bytesRead);
            totalProcessed += bytesRead;

            if (totalProcessed % (CHUNK_SIZE * 10) == 0) {
                std::cout << "\rDecrypting... " << totalProcessed / 1024 << " KB" << std::flush;
            }
        }
    }

    std::cout << "\rDecrypting... Done (" << totalProcessed / 1024 << " KB)\n";
    return true;
}