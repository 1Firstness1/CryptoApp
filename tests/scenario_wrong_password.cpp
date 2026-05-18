#include "CryptoApp.h"
#include "Options.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::cout << "=== Scenario: Wrong Password on Decryption ===\n";

    // Create and encrypt file
    std::ofstream("test.txt") << "Secret data";

    Options encryptOpt;
    encryptOpt.mode = "encrypt";
    encryptOpt.backend = "openssl";
    encryptOpt.input = "test.txt";
    encryptOpt.output = "test.enc";
    encryptOpt.force = true;

    CryptoApp app;
    std::string correctPassword = "CorrectPassword123";

    bool encryptResult = app.processFile(encryptOpt, correctPassword);

    if (!encryptResult) {
        std::cout << "✗ Encryption failed\n";
        return 1;
    }
    std::cout << "✓ Encryption successful\n";

    Options decryptOpt;
    decryptOpt.mode = "decrypt";
    decryptOpt.backend = "openssl";
    decryptOpt.input = "test.enc";
    decryptOpt.output = "test_decrypted.txt";
    decryptOpt.force = true;

    std::string wrongPassword = "WrongPassword456";
    bool decryptResult = app.processFile(decryptOpt, wrongPassword);

    if (!decryptResult) {
        std::cout << "✓ Decryption correctly failed with wrong password\n";
    } else {
        std::cout << "✗ Decryption should have failed but succeeded\n";
        return 1;
    }

    fs::remove("test.txt");
    fs::remove("test.enc");
    fs::remove("test_decrypted.txt");

    std::cout << "=== Test passed ===\n";
    return 0;
}