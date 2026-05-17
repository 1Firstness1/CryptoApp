#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "Options.h"
#include "CryptoApp.h"

enum class Nav { Ok, Back, Menu, Exit };

struct StepResult {
    Nav nav = Nav::Ok;
    std::string value;
};

static std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) ++start;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) --end;
    return s.substr(start, end - start);
}

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

static void printHelp(const char* exe) {
    std::cout
        << "Crypto App\n\n"
        << "CLI mode:\n"
        << "  " << exe << " --mode <encrypt|decrypt> --backend <openssl|simple|o|s> --in <input> [--out <output>] [--force]\n\n"
        << "Options:\n"
        << "  --mode       encrypt | decrypt\n"
        << "  --backend    openssl | simple | o | s\n"
        << "  --in         input file path\n"
        << "  --out        output file path\n"
        << "  --force      overwrite output file\n"
        << "  --help       show help\n\n"
        << "Interactive mode:\n"
        << "  Run without arguments.\n";
}

static std::string normalizeBackend(std::string v) {
    v = lower(trim(v));
    if (v == "o") return "openssl";
    if (v == "s") return "simple";
    return v;
}

static Options parseArgs(int argc, char* argv[]) {
    Options opt;

    auto needValue = [&](int& i, const std::string& key) -> std::string {
        if (i + 1 >= argc) throw std::runtime_error("Missing value for " + key);
        return argv[++i];
    };

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--help" || a == "-h") opt.help = true;
        else if (a == "--mode") opt.mode = needValue(i, a);
        else if (a == "--backend") opt.backend = needValue(i, a);
        else if (a == "--in") opt.input = needValue(i, a);
        else if (a == "--out") opt.output = needValue(i, a);
        else if (a == "--force") opt.force = true;
        else throw std::runtime_error("Unknown argument: " + a);
    }

    if (!opt.help) {
        opt.mode = lower(trim(opt.mode));
        opt.backend = normalizeBackend(opt.backend);
        opt.input = trim(opt.input);
        opt.output = trim(opt.output);

        if (opt.mode != "encrypt" && opt.mode != "decrypt") throw std::runtime_error("Invalid --mode");
        if (opt.backend != "openssl" && opt.backend != "simple") throw std::runtime_error("Invalid --backend");
        if (opt.input.empty()) throw std::runtime_error("Missing --in");
        if (opt.output.empty()) opt.output = autoOutputName(opt);
    }

    return opt;
}

static int askInt(const std::string& prompt, int minV, int maxV) {
    while (true) {
        std::cout << prompt;
        int x;
        if (std::cin >> x && x >= minV && x <= maxV) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input\n";
    }
}

static StepResult askNav(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    s = trim(s);
    std::string l = lower(s);
    if (l == "back") return {Nav::Back, ""};
    if (l == "menu") return {Nav::Menu, ""};
    if (l == "exit") return {Nav::Exit, ""};
    return {Nav::Ok, s};
}

static StepResult askRequired(const std::string& prompt) {
    while (true) {
        auto r = askNav(prompt);
        if (r.nav != Nav::Ok) return r;
        if (!r.value.empty()) return r;
        std::cout << "Empty value not allowed\n";
    }
}

static StepResult askBackend() {
    while (true) {
        auto r = askNav("Backend openssl or simple (o/s): ");
        if (r.nav != Nav::Ok) return r;
        std::string v = normalizeBackend(r.value);
        if (v == "openssl" || v == "simple") return {Nav::Ok, v};
        std::cout << "Enter openssl, simple, o or s\n";
    }
}

static StepResult askYesNoNav(const std::string& prompt, bool def = false) {
    while (true) {
        auto r = askNav(prompt);
        if (r.nav != Nav::Ok) return r;
        std::string s = lower(trim(r.value));
        if (s.empty()) return {Nav::Ok, def ? "yes" : "no"};
        if (s == "y" || s == "yes") return {Nav::Ok, "yes"};
        if (s == "n" || s == "no") return {Nav::Ok, "no"};
        std::cout << "Enter y/yes or n/no\n";
    }
}

static void printMenu() {
    std::cout << "\n=== Crypto App ===\n"
              << "1) Encrypt file\n"
              << "2) Decrypt file\n"
              << "3) Help\n"
              << "4) Exit\n";
}

static void printInteractiveHelp() {
    std::cout << "\nCommands in steps: back, menu, exit\n";
}

static int runCliMode(int argc, char* argv[]) {
    Options opt = parseArgs(argc, argv);
    if (opt.help) {
        printHelp(argv[0]);
        return 0;
    }

    std::string p1 = readPassword("Enter password: ");
    if (p1.empty()) {
        std::cerr << "Error: empty password\n";
        return 2;
    }

    if (opt.mode == "encrypt") {
        std::string p2 = readPassword("Repeat password: ");
        if (p1 != p2) {
            std::cerr << "Error: passwords do not match\n";
            std::fill(p2.begin(), p2.end(), '\0');
            std::fill(p1.begin(), p1.end(), '\0');
            return 2;
        }
        std::fill(p2.begin(), p2.end(), '\0');
    }

    CryptoApp app;
    bool ok = app.processFile(opt, p1);
    std::fill(p1.begin(), p1.end(), '\0');

    if (!ok) return 1;
    std::cout << "Success: " << opt.mode << "\n";
    std::cout << "Output file: " << opt.output << "\n";
    return 0;
}

static int runInteractiveMode() {
    while (true) {
        printMenu();
        int choice = askInt("Select option: ", 1, 4);

        if (choice == 4) return 0;
        if (choice == 3) {
            printHelp("crypto_app");
            printInteractiveHelp();
            continue;
        }

        Options opt;
        opt.mode = (choice == 1) ? "encrypt" : "decrypt";

        enum Step { S_BACKEND = 0, S_INPUT, S_OUTPUT, S_FORCE, S_PASSWORD, S_CONFIRM, S_RUN };
        int step = S_BACKEND;
        bool toMenu = false;
        std::string password;

        printInteractiveHelp();

        while (!toMenu) {
            if (step == S_BACKEND) {
                auto r = askBackend();
                if (r.nav == Nav::Exit) return 0;
                if (r.nav == Nav::Menu) { toMenu = true; continue; }
                if (r.nav == Nav::Back) continue;
                opt.backend = r.value;
                step = S_INPUT;
            } else if (step == S_INPUT) {
                auto r = askRequired("Input file path: ");
                if (r.nav == Nav::Exit) return 0;
                if (r.nav == Nav::Menu) { toMenu = true; continue; }
                if (r.nav == Nav::Back) { step = S_BACKEND; continue; }
                opt.input = trim(r.value);
                step = S_OUTPUT;
            } else if (step == S_OUTPUT) {
                auto r = askNav("Output file path (Enter = auto): ");
                if (r.nav == Nav::Exit) return 0;
                if (r.nav == Nav::Menu) { toMenu = true; continue; }
                if (r.nav == Nav::Back) { step = S_INPUT; continue; }
                opt.output = trim(r.value);
                if (opt.output.empty()) opt.output = autoOutputName(opt);
                step = S_FORCE;
            } else if (step == S_FORCE) {
                auto r = askYesNoNav("Overwrite output file if exists? (y/n): ", false);
                if (r.nav == Nav::Exit) return 0;
                if (r.nav == Nav::Menu) { toMenu = true; continue; }
                if (r.nav == Nav::Back) { step = S_OUTPUT; continue; }
                opt.force = (r.value == "yes");
                step = S_PASSWORD;
            } else if (step == S_PASSWORD) {
                password = readPassword("Enter password: ");
                if (password.empty()) {
                    std::cout << "Empty password not allowed\n";
                    continue;
                }
                step = (opt.mode == "encrypt") ? S_CONFIRM : S_RUN;
            } else if (step == S_CONFIRM) {
                std::string p2 = readPassword("Repeat password: ");
                if (password != p2) {
                    std::cout << "Passwords do not match\n";
                    std::fill(p2.begin(), p2.end(), '\0');
                    continue;
                }
                std::fill(p2.begin(), p2.end(), '\0');
                step = S_RUN;
            } else if (step == S_RUN) {
                CryptoApp app;
                bool ok = app.processFile(opt, password);
                std::fill(password.begin(), password.end(), '\0');

                if (ok) {
                    std::cout << "Success: " << opt.mode << "\n";
                    std::cout << "Output file: " << opt.output << "\n";
                } else {
                    std::cout << "Operation failed\n";
                }
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc > 1) return runCliMode(argc, argv);
        return runInteractiveMode();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 2;
    }
}