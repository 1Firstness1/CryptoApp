#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "Constants.h"

class FileIOService {
public:
    FileIOService();
    
    bool openInput(const std::string& path);
    bool openOutput(const std::string& path, bool force = false);
    void close();
    
    bool readChunk(std::vector<uint8_t>& buffer, size_t& bytesRead);
    bool writeChunk(const std::vector<uint8_t>& data, size_t size);
    
    bool fileExists(const std::string& path) const;
    bool removeOutputOnError();
    
private:
    std::string m_inputPath;
    std::string m_outputPath;
    bool m_outputOpened;
};