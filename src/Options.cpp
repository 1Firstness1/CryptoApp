#include "Options.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

std::string autoOutputName(const Options& opt) {
    if (!opt.output.empty()) return opt.output;
    if (opt.mode == "encrypt") return opt.input + ".enc";
    if (opt.input.size() > 4 && opt.input.substr(opt.input.size() - 4) == ".enc") {
        return opt.input.substr(0, opt.input.size() - 4);
    }
    return opt.input + ".dec";
}

std::string readPassword(const std::string& prompt) {
    std::cout << prompt;
    std::string pwd;

#ifdef _WIN32
    while (true) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            std::cout << "\n";
            break;
        } else if (ch == '\b') {
            if (!pwd.empty()) pwd.pop_back();
        } else if (ch == 3) {
            throw std::runtime_error("Input interrupted.");
        } else if (ch >= 32 && ch <= 126) {
            pwd.push_back(static_cast<char>(ch));
        }
    }
#else
    termios oldt{}, newt{};
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) {
        throw std::runtime_error("Failed to configure terminal for password input.");
    }
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) != 0) {
        throw std::runtime_error("Failed to disable terminal echo.");
    }

    std::getline(std::cin, pwd);
    std::cout << "\n";
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return pwd;
}