#include "CryptoHeader.h"
#include <cstring>
#include <openssl/rand.h>

static constexpr const char* MAGIC = "FENC1";

CryptoHeader::CryptoHeader()
    : m_version(1)
    , m_backend(Backend::OPENSSL)
{
    std::memcpy(m_magic.data(), MAGIC, MAGIC_SIZE);
    m_salt.fill(0);
    m_iv.fill(0);
}

bool CryptoHeader::write(std::ofstream& out) const {
    out.write(m_magic.data(), MAGIC_SIZE);
    out.write(reinterpret_cast<const char*>(&m_version), 1);
    out.write(reinterpret_cast<const char*>(&m_backend), 1);
    out.write(reinterpret_cast<const char*>(m_salt.data()), SALT_SIZE);
    out.write(reinterpret_cast<const char*>(m_iv.data()), IV_SIZE);
    return static_cast<bool>(out);
}

bool CryptoHeader::read(std::ifstream& in) {
    in.read(m_magic.data(), MAGIC_SIZE);
    in.read(reinterpret_cast<char*>(&m_version), 1);
    in.read(reinterpret_cast<char*>(&m_backend), 1);
    in.read(reinterpret_cast<char*>(m_salt.data()), SALT_SIZE);
    in.read(reinterpret_cast<char*>(m_iv.data()), IV_SIZE);
    return static_cast<bool>(in);
}

bool CryptoHeader::validate() const {
    if (std::memcmp(m_magic.data(), MAGIC, MAGIC_SIZE) != 0) return false;
    if (m_version != 1) return false;
    if (m_backend != Backend::OPENSSL && m_backend != Backend::SIMPLE) return false;
    return true;
}

void CryptoHeader::generateRandomSalt() {
    RAND_bytes(m_salt.data(), SALT_SIZE);
}

void CryptoHeader::generateRandomIV() {
    RAND_bytes(m_iv.data(), IV_SIZE);
}