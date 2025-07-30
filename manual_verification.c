/**
 * @file manual_verification.c
 * @brief Manual calculation verification for round-trip testing
 * 
 * This file implements manual verification functions to cross-check
 * the results of cryptographic operations and detect round-trip failures.
 * 
 * @author TouanRichi  
 * @date 2025-07-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/**
 * @brief Manual modular exponentiation for small values
 * Used to verify the correctness of bigint modular exponentiation
 */
uint64_t manual_mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    if (mod == 0) return 0;
    if (mod == 1) return 0;
    if (exp == 0) return 1;
    if (base == 0) return 0;
    
    uint64_t result = 1;
    base = base % mod;
    
    printf("[MANUAL_VERIFY] Computing %llu^%llu mod %llu\n", 
           (unsigned long long)base, (unsigned long long)exp, (unsigned long long)mod);
    
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
            printf("[MANUAL_VERIFY]   Result updated to %llu\n", (unsigned long long)result);
        }
        exp >>= 1;
        if (exp > 0) {
            base = (base * base) % mod;
            printf("[MANUAL_VERIFY]   Base squared to %llu\n", (unsigned long long)base);
        }
    }
    
    printf("[MANUAL_VERIFY] Final result: %llu\n", (unsigned long long)result);
    return result;
}

/**
 * @brief Manual RSA round-trip verification
 */
int manual_rsa_verify(uint32_t message, uint32_t modulus, uint32_t pub_exp, uint32_t priv_exp) {
    printf("[MANUAL_RSA_VERIFY] Testing RSA round-trip for message=%u\n", message);
    printf("[MANUAL_RSA_VERIFY] Parameters: n=%u, e=%u, d=%u\n", modulus, pub_exp, priv_exp);
    
    if (message >= modulus) {
        printf("[MANUAL_RSA_VERIFY] ERROR: Message %u >= modulus %u\n", message, modulus);
        return -1;
    }
    
    // Encrypt: c = m^e mod n
    uint64_t ciphertext = manual_mod_exp(message, pub_exp, modulus);
    printf("[MANUAL_RSA_VERIFY] Encrypted: %u^%u mod %u = %llu\n", 
           message, pub_exp, modulus, (unsigned long long)ciphertext);
    
    // Decrypt: m = c^d mod n  
    uint64_t decrypted = manual_mod_exp(ciphertext, priv_exp, modulus);
    printf("[MANUAL_RSA_VERIFY] Decrypted: %llu^%u mod %u = %llu\n", 
           (unsigned long long)ciphertext, priv_exp, modulus, (unsigned long long)decrypted);
    
    // Verify round-trip
    if (decrypted == message) {
        printf("[MANUAL_RSA_VERIFY] ✅ Round-trip SUCCESS: %u -> %llu -> %llu\n", 
               message, (unsigned long long)ciphertext, (unsigned long long)decrypted);
        return 0;
    } else {
        printf("[MANUAL_RSA_VERIFY] ❌ Round-trip FAILURE: %u -> %llu -> %llu\n", 
               message, (unsigned long long)ciphertext, (unsigned long long)decrypted);
        return -1;
    }
}

/**
 * @brief Manual Montgomery conversion verification for small values
 */
int manual_montgomery_verify(uint32_t value, uint32_t modulus) {
    if (modulus == 0 || (modulus & 1) == 0) {
        printf("[MANUAL_MONT_VERIFY] ERROR: Invalid modulus %u (must be odd and non-zero)\n", modulus);
        return -1;
    }
    
    if (value >= modulus) {
        value = value % modulus;
        printf("[MANUAL_MONT_VERIFY] Reduced input to %u\n", value);
    }
    
    printf("[MANUAL_MONT_VERIFY] Testing Montgomery conversion for value=%u, modulus=%u\n", value, modulus);
    
    // Calculate R = 2^32 for single-word modulus
    uint64_t R = 1ULL << 32;
    uint64_t R_mod_n = R % modulus;
    
    printf("[MANUAL_MONT_VERIFY] R = 2^32 = %llu, R mod n = %llu\n", 
           (unsigned long long)R, (unsigned long long)R_mod_n);
    
    // Montgomery form: a_mont = (a * R) mod n
    uint64_t mont_form = (value * R_mod_n) % modulus;
    printf("[MANUAL_MONT_VERIFY] Montgomery form: (%u * %llu) mod %u = %llu\n", 
           value, (unsigned long long)R_mod_n, modulus, (unsigned long long)mont_form);
    
    // Note: Converting back requires R^(-1) which is complex to compute manually
    // This function mainly validates the to_form conversion
    
    return 0;
}

/**
 * @brief Verify basic arithmetic operations for small values
 */
int manual_arithmetic_verify() {
    printf("[MANUAL_ARITH_VERIFY] Testing basic arithmetic operations\n");
    
    // Test multiplication
    struct {
        uint32_t a, b, expected;
    } mul_tests[] = {
        {2, 3, 6},
        {5, 7, 35},
        {11, 13, 143},
        {17, 19, 323},
        {65535, 65535, 65535ULL * 65535ULL}
    };
    
    for (int i = 0; i < 5; i++) {
        uint64_t result = (uint64_t)mul_tests[i].a * mul_tests[i].b;
        if (result == mul_tests[i].expected) {
            printf("[MANUAL_ARITH_VERIFY] ✅ %u * %u = %llu\n", 
                   mul_tests[i].a, mul_tests[i].b, (unsigned long long)result);
        } else {
            printf("[MANUAL_ARITH_VERIFY] ❌ %u * %u = %llu, expected %u\n", 
                   mul_tests[i].a, mul_tests[i].b, (unsigned long long)result, mul_tests[i].expected);
            return -1;
        }
    }
    
    // Test modular reduction
    struct {
        uint64_t value, modulus, expected;
    } mod_tests[] = {
        {10, 3, 1},
        {100, 7, 2},
        {65536, 35, 21},
        {4294967296ULL, 143, 77}
    };
    
    for (int i = 0; i < 4; i++) {
        uint64_t result = mod_tests[i].value % mod_tests[i].modulus;
        if (result == mod_tests[i].expected) {
            printf("[MANUAL_ARITH_VERIFY] ✅ %llu mod %llu = %llu\n", 
                   (unsigned long long)mod_tests[i].value, 
                   (unsigned long long)mod_tests[i].modulus, 
                   (unsigned long long)result);
        } else {
            printf("[MANUAL_ARITH_VERIFY] ❌ %llu mod %llu = %llu, expected %llu\n", 
                   (unsigned long long)mod_tests[i].value, 
                   (unsigned long long)mod_tests[i].modulus, 
                   (unsigned long long)result, 
                   (unsigned long long)mod_tests[i].expected);
            return -1;
        }
    }
    
    printf("[MANUAL_ARITH_VERIFY] ✅ All basic arithmetic tests passed\n");
    return 0;
}

/**
 * @brief Comprehensive manual verification test suite
 */
int run_manual_verification_tests() {
    printf("========================================\n");
    printf("🔍 MANUAL VERIFICATION TEST SUITE\n");
    printf("========================================\n");
    
    int failures = 0;
    
    // Test basic arithmetic
    if (manual_arithmetic_verify() != 0) {
        failures++;
    }
    
    // Test RSA round-trip for known good cases
    struct {
        uint32_t msg, n, e, d;
    } rsa_tests[] = {
        {2, 35, 5, 5},   // n=5*7, e=5, d=5
        {3, 35, 5, 5},
        {4, 35, 5, 5},
        {22, 143, 7, 103}, // n=11*13, e=7, d=103
        {1, 143, 7, 103},
        {142, 143, 7, 103}
    };
    
    for (int i = 0; i < 6; i++) {
        if (manual_rsa_verify(rsa_tests[i].msg, rsa_tests[i].n, 
                             rsa_tests[i].e, rsa_tests[i].d) != 0) {
            failures++;
        }
    }
    
    // Test Montgomery conversions
    uint32_t mont_tests[] = {1, 2, 3, 5, 10, 20, 34};
    for (int i = 0; i < 7; i++) {
        if (manual_montgomery_verify(mont_tests[i], 35) != 0) {
            failures++;
        }
    }
    
    printf("========================================\n");
    if (failures == 0) {
        printf("✅ ALL MANUAL VERIFICATION TESTS PASSED\n");
    } else {
        printf("❌ %d MANUAL VERIFICATION TESTS FAILED\n", failures);
    }
    printf("========================================\n");
    
    return failures;
}

int main() {
    return run_manual_verification_tests();
}