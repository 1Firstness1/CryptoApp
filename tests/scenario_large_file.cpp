#include "CryptoApp.h"
#include "Options.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <vector>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== Scenario: Large File Processing ===\n";

    // Create a 5MB test file (smaller for faster testing)
    const size_t fileSize = 5 * 1024 * 1024; // 5 MB
    std::ofstream largeFile("large_test.txt", std::ios::binary);

    std::vector<char> data(1024, 'X');
    for (size_t i = 0; i < fileSize / 1024; ++i) {
        largeFile.write(data.data(), data.size());
    }
    largeFile.close();

    std::cout << "✓ Created " << fileSize / (1024*1024) << "MB test file\n";
    
    // Encrypt
    Options encryptOpt;
    encryptOpt.mode = "encrypt";
    encryptOpt.backend = "openssl";
    encryptOpt.input = "large_test.txt";
    encryptOpt.output = "large_test.enc";
    encryptOpt.force = true;
    
    CryptoApp app;
    std::string password = "LargeFilePassword";
    
    auto start = std::chrono::high_resolution_clock::now();
    bool encryptResult = app.processFile(encryptOpt, password);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    if (encryptResult) {
        std::cout << "✓ Encryption completed in " << duration.count() << "ms\n";
    } else {
        std::cout << "✗ Encryption failed\n";
        return 1;
    }
    
    // Decrypt
    Options decryptOpt;
    decryptOpt.mode = "decrypt";
    decryptOpt.backend = "openssl";
    decryptOpt.input = "large_test.enc";
    decryptOpt.output = "large_test_decrypted.txt";
    decryptOpt.force = true;
    
    start = std::chrono::high_resolution_clock::now();
    bool decryptResult = app.processFile(decryptOpt, password);
    end = std::chrono::high_resolution_clock::now();
    
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    if (decryptResult) {
        std::cout << "✓ Decryption completed in " << duration.count() << "ms\n";
    } else {
        std::cout << "✗ Decryption failed\n";
        return 1;
    }
    
    // Verify file sizes
    if (fs::file_size("large_test.txt") == fs::file_size("large_test_decrypted.txt")) {
        std::cout << "✓ File size verification passed\n";
    } else {
        std::cout << "✗ File size mismatch\n";
        return 1;
    }
    
    // Cleanup
    fs::remove("large_test.txt");
    fs::remove("large_test.enc");
    fs::remove("large_test_decrypted.txt");
    
    std::cout << "=== Test passed ===\n";
    return 0;
}