#include <gtest/gtest.h>
#include "CryptoApp.h"
#include "Options.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class CryptoAppTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream("test_input.txt") << "Hello, World! Test content for crypto app.";
        std::ofstream("test_empty.txt") << "";
        password = "TestPassword123";
    }

    void TearDown() override {
        fs::remove("test_input.txt");
        fs::remove("test_empty.txt");
        fs::remove("test_output.enc");
    }

    std::string password;
    CryptoApp app;
};

TEST_F(CryptoAppTest, ProcessFile_EncryptWithOpenSSL_Success) {
    Options opt;
    opt.mode = "encrypt";
    opt.backend = "openssl";
    opt.input = "test_input.txt";
    opt.output = "test_output.enc";
    opt.force = true;

    bool result = app.processFile(opt, password);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists("test_output.enc"));
}

TEST_F(CryptoAppTest, ProcessFile_EncryptWithSimple_Success) {
    Options opt;
    opt.mode = "encrypt";
    opt.backend = "simple";
    opt.input = "test_input.txt";
    opt.output = "test_output.enc";
    opt.force = true;

    bool result = app.processFile(opt, password);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists("test_output.enc"));
}

TEST_F(CryptoAppTest, ProcessFile_InvalidMode_ReturnsFalse) {
    Options opt;
    opt.mode = "invalid";
    opt.backend = "openssl";
    opt.input = "test_input.txt";

    bool result = app.processFile(opt, password);
    EXPECT_FALSE(result);
}

TEST_F(CryptoAppTest, ProcessFile_InvalidBackend_ReturnsFalse) {
    Options opt;
    opt.mode = "encrypt";
    opt.backend = "invalid";
    opt.input = "test_input.txt";

    bool result = app.processFile(opt, password);
    EXPECT_FALSE(result);
}

TEST_F(CryptoAppTest, ProcessFile_EmptyPassword_StillWorks) {
    Options opt;
    opt.mode = "encrypt";
    opt.backend = "openssl";
    opt.input = "test_input.txt";
    opt.output = "test_output.enc";
    opt.force = true;

    bool result = app.processFile(opt, "");
    EXPECT_TRUE(result);
}

TEST_F(CryptoAppTest, ProcessFile_EncryptEmptyFile_Success) {
    Options opt;
    opt.mode = "encrypt";
    opt.backend = "openssl";
    opt.input = "test_empty.txt";
    opt.output = "test_output.enc";
    opt.force = true;

    bool result = app.processFile(opt, password);
    EXPECT_TRUE(result);
    EXPECT_TRUE(fs::exists("test_output.enc"));
}