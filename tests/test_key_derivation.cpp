#include <gtest/gtest.h>
#include "KeyDerivationService.h"
#include "Constants.h"

class KeyDerivationTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_unique<KeyDerivationService>();

        for (size_t i = 0; i < SALT_SIZE; ++i) {
            testSalt[i] = static_cast<uint8_t>(i);
        }
    }

    std::unique_ptr<KeyDerivationService> service;
    std::array<uint8_t, SALT_SIZE> testSalt;
};

TEST_F(KeyDerivationTest, DeriveKey_ValidPassword_ReturnsTrue) {
    std::string password = "MySecurePassword123!";
    std::array<uint8_t, KEY_SIZE> key;

    bool result = service->deriveKey(password, testSalt.data(), SALT_SIZE, key);
    EXPECT_TRUE(result);
}

TEST_F(KeyDerivationTest, DeriveKey_EmptyPassword_ReturnsTrue) {
    std::string password = "";
    std::array<uint8_t, KEY_SIZE> key;

    bool result = service->deriveKey(password, testSalt.data(), SALT_SIZE, key);
    EXPECT_TRUE(result);
}

TEST_F(KeyDerivationTest, DeriveKey_LongPassword_ReturnsTrue) {
    std::string password(1000, 'A');
    std::array<uint8_t, KEY_SIZE> key;

    bool result = service->deriveKey(password, testSalt.data(), SALT_SIZE, key);
    EXPECT_TRUE(result);
}

TEST_F(KeyDerivationTest, DeriveKey_SamePasswordSameSalt_ProducesSameKey) {
    std::string password = "TestPassword";
    std::array<uint8_t, KEY_SIZE> key1;
    std::array<uint8_t, KEY_SIZE> key2;

    service->deriveKey(password, testSalt.data(), SALT_SIZE, key1);
    service->deriveKey(password, testSalt.data(), SALT_SIZE, key2);

    EXPECT_EQ(key1, key2);
}

TEST_F(KeyDerivationTest, DeriveKey_DifferentSalt_ProducesDifferentKeys) {
    std::string password = "TestPassword";
    std::array<uint8_t, SALT_SIZE> salt2;
    std::array<uint8_t, KEY_SIZE> key1;
    std::array<uint8_t, KEY_SIZE> key2;

    for (size_t i = 0; i < SALT_SIZE; ++i) {
        salt2[i] = static_cast<uint8_t>(i + 100);
    }

    service->deriveKey(password, testSalt.data(), SALT_SIZE, key1);
    service->deriveKey(password, salt2.data(), SALT_SIZE, key2);

    EXPECT_NE(key1, key2);
}

TEST_F(KeyDerivationTest, DeriveKey_DifferentPassword_ProducesDifferentKeys) {
    std::string password1 = "Password123";
    std::string password2 = "Password456";
    std::array<uint8_t, KEY_SIZE> key1;
    std::array<uint8_t, KEY_SIZE> key2;

    service->deriveKey(password1, testSalt.data(), SALT_SIZE, key1);
    service->deriveKey(password2, testSalt.data(), SALT_SIZE, key2);

    EXPECT_NE(key1, key2);
}

TEST_F(KeyDerivationTest, DeriveKey_CustomIterations_Works) {
    std::string password = "TestPassword";
    std::array<uint8_t, KEY_SIZE> key;

    bool result = service->deriveKey(password, testSalt.data(), SALT_SIZE, key, 1000);
    EXPECT_TRUE(result);
}