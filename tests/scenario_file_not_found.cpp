#include "CryptoApp.h"
#include "Options.h"
#include <iostream>

int main() {
    std::cout << "=== Scenario: File Not Found ===\n";
    
    Options opt;
    opt.mode = "decrypt";
    opt.backend = "openssl";
    opt.input = "non_existent_file.enc";
    opt.output = "output.txt";
    opt.force = true;
    
    CryptoApp app;
    std::string password = "password";
    
    bool result = app.processFile(opt, password);
    
    if (!result) {
        std::cout << "✓ Correctly failed with non-existent input file\n";
    } else {
        std::cout << "✗ Should have failed but succeeded\n";
        return 1;
    }
    
    std::cout << "=== Test passed ===\n";
    return 0;
}