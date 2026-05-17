#include <gtest/gtest.h>
#include "FileIOService.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class FileIOTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<FileIOService>();
        
        std::ofstream("test_exists.txt") << "content";
        std::ofstream("test_output.txt") << "old content";
    }
    
    void TearDown() override {
        fs::remove("test_exists.txt");
        fs::remove("test_output.txt");
        fs::remove("test_new.txt");
    }
    
    std::unique_ptr<FileIOService> service;
};

TEST_F(FileIOTest, FileExists_ExistingFile_ReturnsTrue) {
    EXPECT_TRUE(service->fileExists("test_exists.txt"));
}

TEST_F(FileIOTest, FileExists_NonExistingFile_ReturnsFalse) {
    EXPECT_FALSE(service->fileExists("non_existing_file.txt"));
}

TEST_F(FileIOTest, FileExists_EmptyPath_ReturnsFalse) {
    EXPECT_FALSE(service->fileExists(""));
}

TEST_F(FileIOTest, OpenOutput_FileNotExists_ReturnsTrue) {
    EXPECT_TRUE(service->openOutput("test_new.txt", false));
}

TEST_F(FileIOTest, OpenOutput_FileExistsNoForce_ReturnsFalse) {
    EXPECT_FALSE(service->openOutput("test_output.txt", false));
}

TEST_F(FileIOTest, OpenOutput_FileExistsWithForce_ReturnsTrue) {
    EXPECT_TRUE(service->openOutput("test_output.txt", true));
}

TEST_F(FileIOTest, OpenInput_ExistingFile_ReturnsTrue) {
    EXPECT_TRUE(service->openInput("test_exists.txt"));
}

TEST_F(FileIOTest, OpenInput_NonExistingFile_ReturnsFalse) {
    EXPECT_FALSE(service->openInput("non_existing.txt"));
}