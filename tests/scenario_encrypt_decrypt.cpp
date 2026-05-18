#include "CryptoApp.h"
#include "Options.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== Scenario: Encrypt and Decrypt File ===\n";

    const std::string testContent = "Hello, World! This is a test file for encryption.";
    std::ofstream("test_original.txt") << testContent;

    Options encryptOpt;
    encryptOpt.mode = "encrypt";
    encryptOpt.backend = "openssl";
    encryptOpt.input = "test_original.txt";
    encryptOpt.output = "test_encrypted.enc";
    encryptOpt.force = true;
    
    CryptoApp app;
    std::string password = "MySecretPassword123";
    
    bool encryptResult = app.processFile(encryptOpt, password);
    
    if (encryptResult) {
        std::cout << "✓ Encryption successful\n";
    } else {
        std::cout << "✗ Encryption failed\n";
        return 1;
    }

    Options decryptOpt;
    decryptOpt.mode = "decrypt";
    decryptOpt.backend = "openssl";
    decryptOpt.input = "test_encrypted.enc";
    decryptOpt.output = "test_decrypted.txt";
    decryptOpt.force = true;
    
    bool decryptResult = app.processFile(decryptOpt, password);
    
    if (decryptResult) {
        std::cout << "✓ Decryption successful\n";
    } else {
        std::cout << "✗ Decryption failed\n";
        return 1;
    }

    std::ifstream decrypted("test_decrypted.txt");
    std::string content((std::istreambuf_iterator<char>(decrypted)),
                         std::istreambuf_iterator<char>());
    
    if (content == testContent) {
        std::cout << "✓ Content verification successful\n";
    } else {
        std::cout << "✗ Content verification failed\n";
        return 1;
    }

    fs::remove("test_original.txt");
    fs::remove("test_encrypted.enc");
    fs::remove("test_decrypted.txt");
    
    std::cout << "=== All tests passed ===\n";
    return 0;
}