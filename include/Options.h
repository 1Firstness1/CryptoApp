#pragma once
#include <string>

struct Options {
    std::string mode;
    std::string backend;
    std::string input;
    std::string output;
    bool force = false;
    bool help = false;
};

std::string autoOutputName(const Options& opt);
std::string readPassword(const std::string& prompt);