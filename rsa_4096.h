/**
 * @file rsa_4096.h
 * @brief Complete RSA-4096 with Full Montgomery REDC - FIXED Production Ready
 * 
 * @author TouanRichi
 * @date 2025-07-29 09:29:15 UTC
 * @version FINAL_COMPLETE_FIXED_v8.2
 */

#ifndef RSA_4096_H
#define RSA_4096_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/* ===================== CONFIGURATION ===================== */

/* BigInt configuration for 4096-bit numbers */
#define BIGINT_WORD_SIZE 32
#define BIGINT_WORD_MASK 0xFFFFFFFFUL
#define BIGINT_4096_WORDS 512  /* Increased to 512 words to handle full RSA-4096 Montgomery multiplication and intermediate results */

/* Montgomery REDC specific constants */
#define MONTGOMERY_R_WORDS 264  /* Doubled to handle larger R values for 4096-bit modulus */

/* Algorithm limits */
#define MAX_DIVISION_ITERATIONS 10000
#define MAX_INVERSE_ITERATIONS 1000

/* Logging levels */
#define LOG_DEBUG 0
#define LOG_INFO  1
#define LOG_ERROR 2

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO
#endif

/* ===================== MACROS ===================== */

#define CHECKPOINT(level, fmt, ...) \
    do { \
        if (level >= LOG_LEVEL) { \
            printf("[%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__); \
            fflush(stdout); \
        } \
    } while(0)

#define ERROR_RETURN(code, fmt, ...) \
    do { \
        CHECKPOINT(LOG_ERROR, "ERROR " fmt, ##__VA_ARGS__); \
        return (code); \
    } while(0)

/* ===================== DATA STRUCTURES ===================== */

/**
 * @brief Big integer representation
 */
typedef struct {
    uint32_t words[BIGINT_4096_WORDS];  /* Little-endian word array */
    int used;                           /* Number of significant words */
    int sign;                          /* 0 = positive, 1 = negative */
} bigint_t;

/**
 * @brief Complete Montgomery REDC context - FIXED
 */
typedef struct {
    bigint_t n;          /* Modulus (must be odd) */
    bigint_t r;          /* R = 2^(32 * n_words) where R > n */
    bigint_t r_squared;  /* R^2 mod n for conversion to Montgomery form */
    bigint_t r_inv;      /* R^(-1) mod n for conversion from Montgomery form */
    uint32_t n_prime;    /* -n^(-1) mod 2^32 for REDC algorithm */
    int n_words;         /* Number of words in modulus */
    int r_words;         /* Number of words in R */
    int is_active;       /* 1 if Montgomery is active, 0 if disabled */
} montgomery_ctx_t;

/**
 * @brief RSA key structure
 */
typedef struct {
    bigint_t n;                    /* Modulus */
    bigint_t exponent;            /* Public or private exponent */
    montgomery_ctx_t mont_ctx;    /* Montgomery REDC context */
    int is_private;               /* 0 = public key, 1 = private key */
} rsa_4096_key_t;

/* ===================== DEBUG UTILITIES ===================== */

void debug_print_bigint(const char *name, const bigint_t *a);
void debug_verify_invariant(const char *step, const bigint_t *value, const bigint_t *modulus);

/* ===================== BIGINT OPERATIONS ===================== */

/* Basic operations */
void bigint_init(bigint_t *a);
void bigint_copy(bigint_t *dst, const bigint_t *src);
void bigint_set_u32(bigint_t *a, uint32_t val);
int bigint_compare(const bigint_t *a, const bigint_t *b);
int bigint_is_zero(const bigint_t *a);
int bigint_is_one(const bigint_t *a);

/* String/binary conversions */
int bigint_from_hex(bigint_t *a, const char *hex);
int bigint_to_hex(const bigint_t *a, char *hex, size_t hex_size);
int bigint_from_binary(bigint_t *a, const uint8_t *data, size_t data_size);
int bigint_to_binary(const bigint_t *a, uint8_t *data, size_t data_size, size_t *bytes_written);
int bigint_from_decimal(bigint_t *a, const char *decimal);
int bigint_to_decimal(const bigint_t *a, char *str, size_t str_size);

/* Bit operations */
int bigint_shift_left(bigint_t *r, const bigint_t *a, int bits);
int bigint_shift_right(bigint_t *r, const bigint_t *a, int bits);
int bigint_get_bit(const bigint_t *a, int bit_pos);
int bigint_bit_length(const bigint_t *a);

/* Arithmetic operations - FIXED */
int bigint_add(bigint_t *r, const bigint_t *a, const bigint_t *b);
int bigint_sub(bigint_t *r, const bigint_t *a, const bigint_t *b);
int bigint_mul(bigint_t *r, const bigint_t *a, const bigint_t *b);
int bigint_div(bigint_t *q, bigint_t *r, const bigint_t *a, const bigint_t *b);
int bigint_mod(bigint_t *r, const bigint_t *a, const bigint_t *m);

/* Extended arithmetic for Montgomery - FIXED */
int bigint_mul_add_word(bigint_t *result, const bigint_t *a, uint32_t b, uint32_t c);
int bigint_add_word(bigint_t *result, const bigint_t *a, uint32_t word);

/* Modular arithmetic - FIXED */
int bigint_mod_exp(bigint_t *result, const bigint_t *base, const bigint_t *exp, const bigint_t *mod);
int mod_inverse_extended_gcd(bigint_t *result, const bigint_t *a, const bigint_t *m);

/* ===================== HYBRID ALGORITHM SELECTION - TERRANTSH MODEL ===================== */

/**
 * @brief Hybrid modular exponentiation with intelligent algorithm selection
 * 
 * This function implements a hybrid approach referencing the Terrantsh RSA4096 model:
 * - Automatically chooses between Montgomery REDC and traditional modular exponentiation
 * - Falls back to traditional algorithm (like terrantsh/RSA4096) if Montgomery is not suitable
 * - Considers modulus size, buffer capacity, and runtime conditions for optimal performance
 * 
 * @param result Output: result of base^exp mod modulus
 * @param base Input base
 * @param exp Input exponent  
 * @param modulus Input modulus
 * @param mont_ctx Montgomery context (can be NULL for traditional-only mode)
 * @return 0 on success, negative on error
 */
int hybrid_mod_exp(bigint_t *result, const bigint_t *base, const bigint_t *exp, 
                   const bigint_t *modulus, const montgomery_ctx_t *mont_ctx);

/* ===================== MONTGOMERY REDC OPERATIONS - FIXED ===================== */

/* Montgomery context management */
int montgomery_ctx_init(montgomery_ctx_t *ctx, const bigint_t *modulus);
void montgomery_ctx_free(montgomery_ctx_t *ctx);
void montgomery_ctx_print_info(const montgomery_ctx_t *ctx);

/* Core Montgomery REDC algorithm - FIXED */
int montgomery_redc(bigint_t *result, const bigint_t *T, const montgomery_ctx_t *ctx);

/* Montgomery arithmetic operations - FIXED */
int montgomery_to_form(bigint_t *result, const bigint_t *a, const montgomery_ctx_t *ctx);
int montgomery_from_form(bigint_t *result, const bigint_t *a, const montgomery_ctx_t *ctx);
int montgomery_mul(bigint_t *result, const bigint_t *a, const bigint_t *b, const montgomery_ctx_t *ctx);
int montgomery_square(bigint_t *result, const bigint_t *a, const montgomery_ctx_t *ctx);
int montgomery_exp(bigint_t *result, const bigint_t *base, const bigint_t *exp, const montgomery_ctx_t *ctx);

/* ===================== RSA OPERATIONS ===================== */

/* Key management */
void rsa_4096_init(rsa_4096_key_t *key);
void rsa_4096_free(rsa_4096_key_t *key);
int rsa_4096_load_key(rsa_4096_key_t *key, const char *n_decimal, const char *e_decimal, int is_private);
int rsa_4096_load_key_binary(rsa_4096_key_t *key, const uint8_t *n_data, size_t n_size,
                            const uint8_t *e_data, size_t e_size, int is_private);

/* Encryption/Decryption */
int rsa_4096_encrypt(const rsa_4096_key_t *pub_key, const char *message_decimal,
                    char *encrypted_hex, size_t encrypted_size);
int rsa_4096_decrypt(const rsa_4096_key_t *priv_key, const char *encrypted_hex,
                    char *message_decimal, size_t message_size);
int rsa_4096_encrypt_binary(const rsa_4096_key_t *pub_key, const uint8_t *message,
                           size_t message_size, uint8_t *encrypted, size_t encrypted_buffer_size,
                           size_t *encrypted_size);
int rsa_4096_decrypt_binary(const rsa_4096_key_t *priv_key, const uint8_t *encrypted,
                           size_t encrypted_size, uint8_t *message, size_t message_buffer_size,
                           size_t *message_size);

/* ===================== TESTING FUNCTIONS ===================== */

int run_verification(void);
int run_binary_verification(void);
int run_benchmarks(void);
int test_large_rsa_keys(void);
int run_manual_key_test(void);
int test_real_rsa_4096(void);

/* Test hybrid algorithm selection */
int test_hybrid_algorithm_selection(void);

/* ===================== HELPER FUNCTIONS - FIXED ===================== */

int extended_gcd_full(bigint_t *result, const bigint_t *a, const bigint_t *m);

/* ===================== NORMALIZATION FUNCTIONS - NEW ===================== */

void bigint_normalize(bigint_t *a);
int bigint_ensure_capacity(bigint_t *a, int min_words);

#endif /* RSA_4096_H */