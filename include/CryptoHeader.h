#pragma once
#include <cstdint>
#include <array>
#include <fstream>
#include "Constants.h"

class CryptoHeader {
public:
    CryptoHeader();

    enum class Backend : uint8_t {
        OPENSSL = 1,
        SIMPLE = 2
    };

    bool write(std::ofstream& out) const;
    bool read(std::ifstream& in);
    bool validate() const;

    // Getters
    Backend getBackend() const { return m_backend; }
    const std::array<uint8_t, SALT_SIZE>& getSalt() const { return m_salt; }
    const std::array<uint8_t, IV_SIZE>& getIV() const { return m_iv; }
    uint8_t getVersion() const { return m_version; }

    // Setters
    void setBackend(Backend backend) { m_backend = backend; }
    void setSalt(const std::array<uint8_t, SALT_SIZE>& salt) { m_salt = salt; }
    void setIV(const std::array<uint8_t, IV_SIZE>& iv) { m_iv = iv; }
    void generateRandomSalt();
    void generateRandomIV();
    
private:
    std::array<char, MAGIC_SIZE> m_magic;
    uint8_t m_version;
    Backend m_backend;
    std::array<uint8_t, SALT_SIZE> m_salt;
    std::array<uint8_t, IV_SIZE> m_iv;
};