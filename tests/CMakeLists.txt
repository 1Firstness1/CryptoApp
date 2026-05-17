# Unit tests
add_executable(crypto_tests
        test_main.cpp
        test_options.cpp
        test_crypto_header.cpp
        test_key_derivation.cpp
        test_file_io.cpp
        test_crypto_app.cpp
)

target_link_libraries(crypto_tests PRIVATE crypto_lib gtest_main)
target_include_directories(crypto_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_test(NAME CryptoUnitTests COMMAND crypto_tests)

# Сценарии интеграционного тестирования (без gtest)
add_executable(scenario_encrypt_decrypt scenario_encrypt_decrypt.cpp)
target_link_libraries(scenario_encrypt_decrypt PRIVATE crypto_lib)

add_executable(scenario_wrong_password scenario_wrong_password.cpp)
target_link_libraries(scenario_wrong_password PRIVATE crypto_lib)

add_executable(scenario_backend_mismatch scenario_backend_mismatch.cpp)
target_link_libraries(scenario_backend_mismatch PRIVATE crypto_lib)

add_executable(scenario_file_not_found scenario_file_not_found.cpp)
target_link_libraries(scenario_file_not_found PRIVATE crypto_lib)

add_executable(scenario_overwrite_protection scenario_overwrite_protection.cpp)
target_link_libraries(scenario_overwrite_protection PRIVATE crypto_lib)

add_executable(scenario_large_file scenario_large_file.cpp)
target_link_libraries(scenario_large_file PRIVATE crypto_lib)

add_executable(scenario_empty_file scenario_empty_file.cpp)
target_link_libraries(scenario_empty_file PRIVATE crypto_lib)

# Добавляем тесты сценариев
add_test(NAME ScenarioEncryptDecrypt COMMAND scenario_encrypt_decrypt)
add_test(NAME ScenarioWrongPassword COMMAND scenario_wrong_password)
add_test(NAME ScenarioBackendMismatch COMMAND scenario_backend_mismatch)
add_test(NAME ScenarioFileNotFound COMMAND scenario_file_not_found)
add_test(NAME ScenarioOverwriteProtection COMMAND scenario_overwrite_protection)
add_test(NAME ScenarioLargeFile COMMAND scenario_large_file)
add_test(NAME ScenarioEmptyFile COMMAND scenario_empty_file)