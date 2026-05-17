#include "CryptoApp.h"
#include "Options.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== Scenario: Overwrite Protection ===\n";
    
    // Create original file
    std::ofstream("test.txt") << "Original content";
    
    // Encrypt first time
    Options opt1;
    opt1.mode = "encrypt";
    opt1.backend = "openssl";
    opt1.input = "test.txt";
    opt1.output = "output.enc";
    opt1.force = true;
    
    CryptoApp app;
    std::string password = "password";
    
    bool result1 = app.processFile(opt1, password);
    
    if (!result1) {
        std::cout << "✗ First encryption failed\n";
        return 1;
    }
    std::cout << "✓ First encryption successful\n";
    
    // Try to encrypt again without force
    Options opt2;
    opt2.mode = "encrypt";
    opt2.backend = "openssl";
    opt2.input = "test.txt";
    opt2.output = "output.enc";
    opt2.force = false;
    
    bool result2 = app.processFile(opt2, password);
    
    if (!result2) {
        std::cout << "✓ Correctly prevented overwrite without force flag\n";
    } else {
        std::cout << "✗ Should have prevented overwrite but didn't\n";
        return 1;
    }
    
    // Encrypt with force flag
    Options opt3;
    opt3.mode = "encrypt";
    opt3.backend = "openssl";
    opt3.input = "test.txt";
    opt3.output = "output.enc";
    opt3.force = true;
    
    bool result3 = app.processFile(opt3, password);
    
    if (result3) {
        std::cout << "✓ Successfully overwrote with force flag\n";
    } else {
        std::cout << "✗ Should have overwritten but failed\n";
        return 1;
    }
    
    // Cleanup
    fs::remove("test.txt");
    fs::remove("output.enc");
    
    std::cout << "=== Test passed ===\n";
    return 0;
}