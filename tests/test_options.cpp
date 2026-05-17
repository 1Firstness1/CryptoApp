#include <gtest/gtest.h>
#include "Options.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class OptionsTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream("test_input.txt") << "test content";
        std::ofstream("test.enc") << "encrypted content";
    }

    void TearDown() override {
        fs::remove("test_input.txt");
        fs::remove("test.enc");
        fs::remove("test_input.txt.enc");
        fs::remove("test_input.txt.dec");
        fs::remove("custom_output.bin");
    }
};

TEST_F(OptionsTest, AutoOutputName_EncryptMode_AddsEncExtension) {
    Options opt;
    opt.mode = "encrypt";
    opt.input = "test_input.txt";
    opt.output = "";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, "test_input.txt.enc");
}

TEST_F(OptionsTest, AutoOutputName_DecryptMode_RemovesEncExtension) {
    Options opt;
    opt.mode = "decrypt";
    opt.input = "test.enc";
    opt.output = "";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, "test");
}

TEST_F(OptionsTest, AutoOutputName_DecryptMode_AddsDecExtension) {
    Options opt;
    opt.mode = "decrypt";
    opt.input = "test_input.txt";
    opt.output = "";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, "test_input.txt.dec");
}

TEST_F(OptionsTest, AutoOutputName_UserSpecifiedOutput_ReturnsUserOutput) {
    Options opt;
    opt.mode = "encrypt";
    opt.input = "test_input.txt";
    opt.output = "custom_output.bin";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, "custom_output.bin");
}

TEST_F(OptionsTest, AutoOutputName_EmptyInput_ReturnsEmpty) {
    Options opt;
    opt.mode = "encrypt";
    opt.input = "";
    opt.output = "";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, ".enc");
}

TEST_F(OptionsTest, AutoOutputName_DecryptLongPath_HandlesCorrectly) {
    Options opt;
    opt.mode = "decrypt";
    opt.input = "/path/to/long/file.name.with.dots.enc";
    opt.output = "";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, "/path/to/long/file.name.with.dots");
}

TEST_F(OptionsTest, AutoOutputName_EncryptWithExistingOutput_UserSpecifiedPriority) {
    Options opt;
    opt.mode = "encrypt";
    opt.input = "test_input.txt";
    opt.output = "custom_output.bin";

    std::string result = autoOutputName(opt);
    EXPECT_EQ(result, "custom_output.bin");
}