#include "CryptoApp.h"
#include "Options.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== Scenario: Backend Mismatch ===\n";
    
    // Create and encrypt with OpenSSL
    std::ofstream("test.txt") << "Secret data";
    
    Options encryptOpt;
    encryptOpt.mode = "encrypt";
    encryptOpt.backend = "openssl";
    encryptOpt.input = "test.txt";
    encryptOpt.output = "test.enc";
    encryptOpt.force = true;
    
    CryptoApp app;
    std::string password = "TestPassword123";
    
    bool encryptResult = app.processFile(encryptOpt, password);
    
    if (!encryptResult) {
        std::cout << "✗ Encryption failed\n";
        return 1;
    }
    std::cout << "✓ Encryption with OpenSSL successful\n";
    
    // Try to decrypt with Simple backend
    Options decryptOpt;
    decryptOpt.mode = "decrypt";
    decryptOpt.backend = "simple";  // Wrong backend!
    decryptOpt.input = "test.enc";
    decryptOpt.output = "test_decrypted.txt";
    decryptOpt.force = true;
    
    bool decryptResult = app.processFile(decryptOpt, password);
    
    if (!decryptResult) {
        std::cout << "✓ Decryption correctly failed due to backend mismatch\n";
    } else {
        std::cout << "✗ Decryption should have failed but succeeded\n";
        return 1;
    }
    
    // Cleanup
    fs::remove("test.txt");
    fs::remove("test.enc");
    
    std::cout << "=== Test passed ===\n";
    return 0;
}