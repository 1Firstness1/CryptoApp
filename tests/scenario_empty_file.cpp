#include "CryptoApp.h"
#include "Options.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== Scenario: Empty File Processing ===\n";
    
    // Create empty file
    std::ofstream("empty.txt") << "";
    std::cout << "✓ Created empty file\n";
    
    // Encrypt empty file
    Options encryptOpt;
    encryptOpt.mode = "encrypt";
    encryptOpt.backend = "openssl";
    encryptOpt.input = "empty.txt";
    encryptOpt.output = "empty.enc";
    encryptOpt.force = true;
    
    CryptoApp app;
    std::string password = "password";
    
    bool encryptResult = app.processFile(encryptOpt, password);
    
    if (encryptResult) {
        std::cout << "✓ Empty file encryption successful\n";
    } else {
        std::cout << "✗ Empty file encryption failed\n";
        return 1;
    }
    
    // Decrypt empty file
    Options decryptOpt;
    decryptOpt.mode = "decrypt";
    decryptOpt.backend = "openssl";
    decryptOpt.input = "empty.enc";
    decryptOpt.output = "empty_decrypted.txt";
    decryptOpt.force = true;
    
    bool decryptResult = app.processFile(decryptOpt, password);
    
    if (decryptResult) {
        std::cout << "✓ Empty file decryption successful\n";
    } else {
        std::cout << "✗ Empty file decryption failed\n";
        return 1;
    }
    
    // Verify decrypted file is empty
    if (fs::file_size("empty_decrypted.txt") == 0) {
        std::cout << "✓ Decrypted file is empty as expected\n";
    } else {
        std::cout << "✗ Decrypted file is not empty\n";
        return 1;
    }
    
    // Cleanup
    fs::remove("empty.txt");
    fs::remove("empty.enc");
    fs::remove("empty_decrypted.txt");
    
    std::cout << "=== Test passed ===\n";
    return 0;
}