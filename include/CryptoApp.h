#pragma once
#include "Options.h"
#include "ICryptoProvider.h"
#include "KeyDerivationService.h"
#include "FileIOService.h"
#include "CryptoHeader.h"
#include <memory>

class CryptoApp {
public:
    CryptoApp();
    ~CryptoApp() = default;

    bool processFile(const Options& opt, const std::string& password);

    static std::unique_ptr<ICryptoProvider> createProvider(const std::string& backend);

private:
    bool processEncrypt(const Options& opt, const std::string& password);
    bool processDecrypt(const Options& opt, const std::string& password);

    bool validateOptions(const Options& opt) const;
    bool checkFiles(const Options& opt, std::string& outPath) const;

    KeyDerivationService m_kdf;
    FileIOService m_io;
};