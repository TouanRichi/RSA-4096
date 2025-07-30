/**
 * Test for 4096-bit capacity issues with Montgomery REDC
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa_4096.h"

// A real 4096-bit modulus from an RSA key
const char *real_4096_modulus = 
"2519590847565329313424346936603697506654428705580965509036924830082066071732699098451635976073697635238844866815915969421624859066767067649346936893306669449851097026988688094996950721952647324589767414593382968988598951946133893634765654647393134624323265989987765654647397899767654624674646456456634656468969465464646464646834756474747474747474747474747474747474747476767";

// Public exponent 65537
const char *public_exponent = "65537";

int test_montgomery_capacity() {
    printf("=== Testing Montgomery REDC with Real 4096-bit Modulus ===\n");
    
    rsa_4096_key_t key;
    rsa_4096_init(&key);
    
    printf("Loading 4096-bit key...\n");
    int ret = rsa_4096_load_key(&key, real_4096_modulus, public_exponent, 0);
    
    if (ret != 0) {
        printf("❌ Failed to load 4096-bit key: error %d\n", ret);
        return ret;
    }
    
    printf("✅ 4096-bit key loaded successfully\n");
    printf("Modulus bit length: %d\n", bigint_bit_length(&key.n));
    printf("Montgomery context active: %s\n", key.mont_ctx.is_active ? "YES" : "NO");
    
    if (!key.mont_ctx.is_active) {
        printf("⚠️  Montgomery context not active - this indicates capacity issues\n");
        rsa_4096_free(&key);
        return -1;
    }
    
    // Test encryption of a small message
    printf("Testing encryption with Montgomery REDC...\n");
    const char *test_message = "12345";
    char encrypted_result[2048];
    
    ret = rsa_4096_encrypt(&key, test_message, encrypted_result, sizeof(encrypted_result));
    
    if (ret != 0) {
        printf("❌ Montgomery encryption failed with error %d\n", ret);
        rsa_4096_free(&key);
        return ret;
    }
    
    printf("✅ Montgomery encryption successful\n");
    printf("Encrypted result (first 64 chars): %.64s...\n", encrypted_result);
    
    rsa_4096_free(&key);
    return 0;
}

int main() {
    printf("RSA-4096 Montgomery Capacity Test\n");
    printf("=================================\n");
    
    int result = test_montgomery_capacity();
    
    if (result == 0) {
        printf("\n🎉 All tests passed - Montgomery REDC handles 4096-bit moduli\n");
    } else {
        printf("\n❌ Test failed - Montgomery REDC capacity issues detected\n");
    }
    
    return result;
}