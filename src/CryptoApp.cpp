#include "CryptoApp.h"
#include "SimpleAesProvider.h"
#include "OpenSslProvider.h"
#include "Constants.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

CryptoApp::CryptoApp() = default;

std::unique_ptr<ICryptoProvider> CryptoApp::createProvider(const std::string& backend) {
    if (backend == "openssl") {
        return std::make_unique<OpenSslProvider>();
    } else if (backend == "simple") {
        return std::make_unique<SimpleAesProvider>();
    }
    return nullptr;
}

bool CryptoApp::validateOptions(const Options& opt) const {
    if (opt.mode != "encrypt" && opt.mode != "decrypt") {
        std::cerr << "Error: invalid mode\n";
        return false;
    }
    if (opt.backend != "openssl" && opt.backend != "simple") {
        std::cerr << "Error: invalid backend\n";
        return false;
    }
    return true;
}

bool CryptoApp::checkFiles(const Options& opt, std::string& outPath) const {
    outPath = autoOutputName(opt);

    if (!m_io.fileExists(opt.input)) {
        std::cerr << "Error: input file does not exist: " << opt.input << "\n";
        return false;
    }

    if (m_io.fileExists(outPath) && !opt.force) {
        std::cerr << "Error: output file already exists: " << outPath << "\n";
        return false;
    }

    return true;
}

bool CryptoApp::processEncrypt(const Options& opt, const std::string& password) {
    std::string outPath;
    if (!checkFiles(opt, outPath)) return false;

    // Generate salt and IV
    CryptoHeader header;
    header.setBackend(opt.backend == "openssl" ? CryptoHeader::Backend::OPENSSL : CryptoHeader::Backend::SIMPLE);
    header.generateRandomSalt();
    header.generateRandomIV();

    // Derive key from password
    std::array<uint8_t, KEY_SIZE> key;
    if (!m_kdf.deriveKey(password, header.getSalt().data(), SALT_SIZE, key)) {
        std::cerr << "Error: key derivation failed\n";
        return false;
    }

    // Create temporary file for encrypted data
    std::string tempPath = outPath + ".tmp";

    // Encrypt using selected provider to temp file
    auto provider = createProvider(opt.backend);
    if (!provider) {
        std::cerr << "Error: failed to create provider\n";
        return false;
    }

    std::cout << "Starting encryption using " << opt.backend << " backend...\n";
    bool result = provider->encrypt(opt.input, tempPath, key, header.getIV());

    if (!result) {
        std::cerr << "Error: encryption failed\n";
        std::fill(key.begin(), key.end(), 0);
        fs::remove(tempPath);
        return false;
    }

    // Now combine header and encrypted data into final file
    std::ofstream out(outPath, std::ios::binary);
    if (!out) {
        std::cerr << "Error: cannot create output file\n";
        fs::remove(tempPath);
        return false;
    }

    // Write header
    if (!header.write(out)) {
        std::cerr << "Error: failed to write header\n";
        out.close();
        fs::remove(tempPath);
        fs::remove(outPath);
        return false;
    }

    // Append encrypted data
    std::ifstream in(tempPath, std::ios::binary);
    if (!in) {
        std::cerr << "Error: cannot read encrypted data\n";
        out.close();
        fs::remove(tempPath);
        fs::remove(outPath);
        return false;
    }

    out << in.rdbuf();
    in.close();
    out.close();

    // Clean up temp file
    fs::remove(tempPath);

    // Securely clear key from memory
    std::fill(key.begin(), key.end(), 0);

    return true;
}

bool CryptoApp::processDecrypt(const Options& opt, const std::string& password) {
    // Read header from encrypted file
    std::ifstream in(opt.input, std::ios::binary);
    if (!in) {
        std::cerr << "Error: cannot open input file\n";
        return false;
    }

    CryptoHeader header;
    if (!header.read(in)) {
        std::cerr << "Error: invalid encrypted file format\n";
        return false;
    }

    if (!header.validate()) {
        std::cerr << "Error: header validation failed\n";
        return false;
    }

    // Check backend mismatch
    CryptoHeader::Backend requestedBackend = (opt.backend == "openssl")
        ? CryptoHeader::Backend::OPENSSL
        : CryptoHeader::Backend::SIMPLE;

    if (requestedBackend != header.getBackend()) {
        std::cerr << "Error: backend mismatch (file uses "
                  << (header.getBackend() == CryptoHeader::Backend::OPENSSL ? "openssl" : "simple")
                  << ", but requested " << opt.backend << ")\n";
        return false;
    }

    // Derive key from password using salt from header
    std::array<uint8_t, KEY_SIZE> key;
    if (!m_kdf.deriveKey(password, header.getSalt().data(), SALT_SIZE, key)) {
        std::cerr << "Error: key derivation failed\n";
        return false;
    }

    // Generate output path
    std::string outPath = autoOutputName(opt);
    if (m_io.fileExists(outPath) && !opt.force) {
        std::cerr << "Error: output file already exists: " << outPath << "\n";
        return false;
    }

    // Extract encrypted data (skip header)
    // Get current position after header
    std::streampos dataStart = in.tellg();

    // Write encrypted data to temp file
    std::string tempPath = outPath + ".tmp";
    std::ofstream tempOut(tempPath, std::ios::binary);
    if (!tempOut) {
        std::cerr << "Error: cannot create temp file\n";
        return false;
    }

    tempOut << in.rdbuf();
    tempOut.close();
    in.close();

    // Decrypt using selected provider
    auto provider = createProvider(opt.backend);
    if (!provider) {
        std::cerr << "Error: failed to create provider\n";
        fs::remove(tempPath);
        return false;
    }

    std::cout << "Starting decryption using " << opt.backend << " backend...\n";
    bool result = provider->decrypt(tempPath, outPath, key, header.getIV());

    // Clean up temp file
    fs::remove(tempPath);

    // Securely clear key from memory
    std::fill(key.begin(), key.end(), 0);

    if (result) {
        std::cout << "Decryption completed successfully\n";
    }

    return result;
}

bool CryptoApp::processFile(const Options& opt, const std::string& password) {
    if (!validateOptions(opt)) return false;

    if (opt.mode == "encrypt") {
        return processEncrypt(opt, password);
    } else {
        return processDecrypt(opt, password);
    }
}