/**
 * @file rsa_4096_arithmetic.c
 * @brief Complete Arithmetic Operations for RSA-4096 - BUGS FIXED
 * 
 * @author TouanRichi
 * @date 2025-07-29 09:33:15 UTC
 * @version FINAL_COMPLETE_FIXED_v8.2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "rsa_4096.h"

/* ===================== FIXED MODULAR EXPONENTIATION ===================== */

int bigint_mod_exp(bigint_t *result, const bigint_t *base, const bigint_t *exp, const bigint_t *mod) {
    if (result == NULL || base == NULL || exp == NULL || mod == NULL) {
        return -1;
    }
    
    if (bigint_is_zero(mod)) {
        return -2;
    }
    
    if (bigint_is_zero(exp)) {
        bigint_set_u32(result, 1);
        return 0;
    }
    
    if (bigint_is_zero(base)) {
        bigint_init(result);
        return 0;
    }
    
    printf("[MOD_EXP_COMPLETE] Computing %d-word^%d-word mod %d-word\n", 
           base->used, exp->used, mod->used);
    
    /* Optimized exponentiation with sliding window for large exponents */
    if (exp->used > 20) {
        printf("[MOD_EXP_COMPLETE] Very large exponent (%d words), using 4-bit sliding window\n", exp->used);
        
        /* Use 4-bit sliding window for very large exponents */
        bigint_t temp_result, temp_base, window_powers[16];
        bigint_set_u32(&temp_result, 1);
        
        /* Reduce base mod modulus first */
        int ret = bigint_mod(&temp_base, base, mod);
        if (ret != 0) return ret;
        
        /* Precompute powers: base^0, base^1, ..., base^15 */
        bigint_set_u32(&window_powers[0], 1);
        bigint_copy(&window_powers[1], &temp_base);
        
        for (int i = 2; i < 16; i++) {
            bigint_t temp_mult;
            ret = bigint_mul(&temp_mult, &window_powers[i-1], &temp_base);
            if (ret != 0) return ret;
            ret = bigint_mod(&window_powers[i], &temp_mult, mod);
            if (ret != 0) return ret;
        }
        
        printf("[MOD_EXP_COMPLETE] Precomputed 16 window powers\n");
        
        /* Process exponent in 4-bit windows from MSB to LSB */
        int exp_bits = bigint_bit_length(exp);
        int processed_bits = 0;
        
        /* FIXED: Start with first non-zero window */
        int started = 0;
        for (int bit_pos = exp_bits - 1; bit_pos >= 0; bit_pos -= 4) {
            /* Extract 4-bit window */
            int window = 0;
            int actual_bits = 0;
            for (int j = 0; j < 4 && bit_pos - j >= 0; j++) {
                if (bigint_get_bit(exp, bit_pos - j)) {
                    window |= (1 << (3 - j));
                }
                actual_bits++;
            }
            
            if (!started && window == 0) {
                /* Skip leading zero windows */
                continue;
            }
            
            if (!started) {
                /* First non-zero window: just set result to window power */
                bigint_copy(&temp_result, &window_powers[window]);
                started = 1;
            } else {
                /* Square result for each bit in window */
                for (int s = 0; s < actual_bits; s++) {
                    bigint_t temp_square;
                    ret = bigint_mul(&temp_square, &temp_result, &temp_result);
                    if (ret != 0) return ret;
                    ret = bigint_mod(&temp_result, &temp_square, mod);
                    if (ret != 0) return ret;
                }
                
                /* Multiply by window power if window is non-zero */
                if (window > 0) {
                    bigint_t temp_mult;
                    ret = bigint_mul(&temp_mult, &temp_result, &window_powers[window]);
                    if (ret != 0) return ret;
                    ret = bigint_mod(&temp_result, &temp_mult, mod);
                    if (ret != 0) return ret;
                }
            }
            
            processed_bits += actual_bits;
            if (processed_bits % 200 == 0) {
                printf("[MOD_EXP_COMPLETE] Progress: %d/%d bits processed (%.1f%%)\n", 
                       processed_bits, exp_bits, (100.0 * processed_bits) / exp_bits);
            }
        }
        
        bigint_copy(result, &temp_result);
        printf("[MOD_EXP_COMPLETE] Sliding window method completed\n");
        return 0;
    }
    
    /* Standard right-to-left binary method for smaller exponents */
    bigint_t temp_result, temp_base, temp_exp;
    bigint_set_u32(&temp_result, 1);
    bigint_init(&temp_base);
    bigint_init(&temp_exp);
    
    /* Reduce base mod modulus first */
    int ret = bigint_mod(&temp_base, base, mod);
    if (ret != 0) return ret;
    
    /* Copy exponent for processing */
    bigint_copy(&temp_exp, exp);
    
    printf("[MOD_EXP_COMPLETE] Starting right-to-left binary method\n");
    printf("[MOD_EXP_COMPLETE] Base: %d words, Exp: %d words, Mod: %d words\n", 
           temp_base.used, temp_exp.used, mod->used);
    
    int bit_count = 0;
    while (!bigint_is_zero(&temp_exp)) {
        /* Check if current bit is 1 */
        if (temp_exp.words[0] & 1) {
            if (bit_count < 10 || bit_count % 50 == 0) {
                printf("[MOD_EXP_COMPLETE] Bit %d is 1, multiplying result by base\n", bit_count);
            }
            
            bigint_t new_result, product;
            bigint_init(&new_result);
            bigint_init(&product);
            printf("[DEBUG] About to multiply: temp_result.used=%d, temp_base.used=%d\n", temp_result.used, temp_base.used);
            ret = bigint_mul(&product, &temp_result, &temp_base);
            printf("[DEBUG] Multiplication result: %d\n", ret);
            if (ret != 0) return ret;
            
            printf("[DEBUG] About to mod: product.used=%d, mod.used=%d\n", product.used, mod->used);
            ret = bigint_mod(&new_result, &product, mod);
            printf("[DEBUG] Mod result: %d\n", ret);
            if (ret != 0) return ret;
            
            bigint_copy(&temp_result, &new_result);
        }
        
        /* Right shift exponent by 1 bit - FIXED */
        bigint_t new_exp;
        bigint_init(&new_exp);
        ret = bigint_shift_right(&new_exp, &temp_exp, 1);
        if (ret != 0) return ret;
        bigint_copy(&temp_exp, &new_exp);
        
        /* Square the base for next iteration - FIXED: Only if exponent is not zero */
        if (!bigint_is_zero(&temp_exp)) {
            bigint_t new_base, square;
            bigint_init(&new_base);
            bigint_init(&square);
            printf("[DEBUG] About to square: temp_base.used=%d\n", temp_base.used);
            
            /* Prevent runaway growth */
            if (temp_base.used > mod->used + 10) {
                printf("[DEBUG] temp_base too large, aborting\n");
                return -3;
            }
            
            ret = bigint_mul(&square, &temp_base, &temp_base);
            printf("[DEBUG] Squaring result: %d\n", ret);
            if (ret != 0) return ret;
            
            /* Check if square result is reasonable */
            if (square.used > 2 * mod->used + 10) {
                printf("[DEBUG] square result too large, aborting\n");
                return -3;
            }
            
            printf("[DEBUG] About to mod square: square.used=%d, mod.used=%d\n", square.used, mod->used);
            ret = bigint_mod(&new_base, &square, mod);
            printf("[DEBUG] Mod square result: %d, new_base.used=%d\n", ret, new_base.used);
            if (ret != 0) return ret;
            
            bigint_copy(&temp_base, &new_base);
            printf("[DEBUG] After copy: temp_base.used=%d\n", temp_base.used);
            
            bigint_copy(&temp_base, &new_base);
        }
        
        bit_count++;
        
        /* Progress reporting for large computations */
        if (bit_count % 100 == 0) {
            printf("[MOD_EXP_COMPLETE] Progress: bit %d processed\n", bit_count);
        }
        
        /* Safety check */
        if (bit_count > 50000) {
            printf("[MOD_EXP_COMPLETE] ERROR: Too many iterations, aborting\n");
            return -3;
        }
    }
    
    bigint_copy(result, &temp_result);
    
    printf("[MOD_EXP_COMPLETE] Completed in %d iterations\n", bit_count);
    return 0;
}

/* ===================== EXTENDED ARITHMETIC FOR MONTGOMERY - FIXED ===================== */

int bigint_mul_add_word(bigint_t *result, const bigint_t *a, uint32_t b, uint32_t c) {
    if (result == NULL || a == NULL) {
        return -1;
    }
    
    bigint_init(result);
    uint64_t carry = c;  /* Start with c */
    
    /* Compute a * b + c */
    int max_words = a->used + 2; /* Extra space for overflow */
    if (max_words > BIGINT_4096_WORDS) {
        max_words = BIGINT_4096_WORDS;
    }
    
    for (int i = 0; i < max_words && (i < a->used || carry > 0); i++) {
        uint64_t word_product = 0;
        if (i < a->used) {
            word_product = (uint64_t)a->words[i] * b;
        }
        
        uint64_t sum = word_product + carry;
        result->words[i] = (uint32_t)(sum & 0xFFFFFFFF);
        carry = sum >> 32;
        
        result->used = i + 1;
    }
    
    /* FIXED: Handle remaining carry overflow */
    if (carry > 0 && result->used < BIGINT_4096_WORDS) {
        result->words[result->used] = (uint32_t)carry;
        result->used++;
    } else if (carry > 0) {
        return -2;  /* Overflow - this is important for Montgomery */
    }
    
    bigint_normalize(result);
    return 0;
}

int bigint_add_word(bigint_t *result, const bigint_t *a, uint32_t word) {
    if (result == NULL || a == NULL) {
        return -1;
    }
    
    bigint_copy(result, a);
    
    if (result->used == 0) {
        result->used = 1;
        result->words[0] = word;
        return 0;
    }
    
    uint64_t carry = word;
    int i = 0;
    
    /* FIXED: Propagate carry through all words */
    while (carry > 0 && i < BIGINT_4096_WORDS) {
        uint64_t sum;
        if (i < result->used) {
            sum = (uint64_t)result->words[i] + carry;
            result->words[i] = (uint32_t)(sum & 0xFFFFFFFF);
        } else {
            /* Extend the number */
            result->words[i] = (uint32_t)carry;
            result->used = i + 1;
            sum = carry;
        }
        carry = sum >> 32;
        i++;
    }
    
    /* FIXED: Check for overflow */
    if (carry > 0) {
        return -2;  /* Overflow */
    }
    
    bigint_normalize(result);
    return 0;
}

/* ===================== COMPLETE MODULAR INVERSE - FIXED ===================== */

int mod_inverse_extended_gcd(bigint_t *result, const bigint_t *a, const bigint_t *m) {
    /* Use the complete extended GCD implementation - NO CHANGES to algorithm */
    return extended_gcd_full(result, a, m);
}