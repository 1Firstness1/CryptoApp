#include <gtest/gtest.h>
#include "CryptoHeader.h"
#include "Constants.h"
#include <fstream>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

class CryptoHeaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        header = std::make_unique<CryptoHeader>();
    }

    void TearDown() override {
        fs::remove("test_header.bin");
    }

    std::unique_ptr<CryptoHeader> header;
};

TEST_F(CryptoHeaderTest, DefaultConstructor_SetsCorrectMagic) {
    EXPECT_EQ(header->getVersion(), 1);
}

TEST_F(CryptoHeaderTest, SetBackend_OpenSSL_SetsCorrectBackend) {
    header->setBackend(CryptoHeader::Backend::OPENSSL);
    EXPECT_EQ(header->getBackend(), CryptoHeader::Backend::OPENSSL);
}

TEST_F(CryptoHeaderTest, SetBackend_Simple_SetsCorrectBackend) {
    header->setBackend(CryptoHeader::Backend::SIMPLE);
    EXPECT_EQ(header->getBackend(), CryptoHeader::Backend::SIMPLE);
}

TEST_F(CryptoHeaderTest, GenerateRandomSalt_GeneratesNonZeroSalt) {
    std::array<uint8_t, SALT_SIZE> salt;
    salt.fill(0);
    header->setSalt(salt);
    header->generateRandomSalt();

    const auto& newSalt = header->getSalt();
    bool allZero = true;
    for (auto byte : newSalt) {
        if (byte != 0) {
            allZero = false;
            break;
        }
    }
    EXPECT_FALSE(allZero);
}

TEST_F(CryptoHeaderTest, GenerateRandomIV_GeneratesNonZeroIV) {
    std::array<uint8_t, IV_SIZE> iv;
    iv.fill(0);
    header->setIV(iv);
    header->generateRandomIV();

    const auto& newIV = header->getIV();
    bool allZero = true;
    for (auto byte : newIV) {
        if (byte != 0) {
            allZero = false;
            break;
        }
    }
    EXPECT_FALSE(allZero);
}

TEST_F(CryptoHeaderTest, WriteAndRead_ValidHeader_RoundtripSuccess) {
    header->setBackend(CryptoHeader::Backend::OPENSSL);
    header->generateRandomSalt();
    header->generateRandomIV();

    std::ofstream out("test_header.bin", std::ios::binary);
    ASSERT_TRUE(header->write(out));
    out.close();

    CryptoHeader newHeader;
    std::ifstream in("test_header.bin", std::ios::binary);
    ASSERT_TRUE(newHeader.read(in));
    in.close();

    EXPECT_EQ(newHeader.getBackend(), header->getBackend());
    EXPECT_EQ(newHeader.getVersion(), header->getVersion());
    EXPECT_EQ(newHeader.getSalt(), header->getSalt());
    EXPECT_EQ(newHeader.getIV(), header->getIV());
}

TEST_F(CryptoHeaderTest, Validate_ValidHeader_ReturnsTrue) {
    header->setBackend(CryptoHeader::Backend::OPENSSL);
    header->generateRandomSalt();
    header->generateRandomIV();

    EXPECT_TRUE(header->validate());
}

TEST_F(CryptoHeaderTest, Validate_InvalidMagic_ReturnsFalse) {
    std::ofstream out("test_header.bin", std::ios::binary);
    out.write("BAD", 3);
    out.close();
    
    CryptoHeader newHeader;
    std::ifstream in("test_header.bin", std::ios::binary);
    newHeader.read(in);
    in.close();
    
    EXPECT_FALSE(newHeader.validate());
}