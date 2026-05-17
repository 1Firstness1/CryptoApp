#include "FileIOService.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

FileIOService::FileIOService()
    : m_outputOpened(false) {
}

bool FileIOService::openInput(const std::string& path) {
    m_inputPath = path;
    return fileExists(path);
}

bool FileIOService::openOutput(const std::string& path, bool force) {
    m_outputPath = path;
    m_outputOpened = true;
    
    if (fileExists(path) && !force) {
        std::cerr << "Error: output file already exists: " << path << "\n";
        return false;
    }
    
    return true;
}

void FileIOService::close() {
    m_outputOpened = false;
}

bool FileIOService::readChunk(std::vector<uint8_t>& buffer, size_t& bytesRead) {
    // Actual reading is done in provider classes
    bytesRead = 0;
    return true;
}

bool FileIOService::writeChunk(const std::vector<uint8_t>& data, size_t size) {
    return true;
}

bool FileIOService::fileExists(const std::string& path) const {
    std::error_code ec;
    return fs::exists(path, ec);
}

bool FileIOService::removeOutputOnError() {
    if (m_outputOpened && !m_outputPath.empty()) {
        std::error_code ec;
        return fs::remove(m_outputPath, ec);
    }
    return true;
}