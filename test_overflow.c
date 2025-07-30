#include <stdio.h>
#include "rsa_4096.h"

int main() {
    printf("Testing big integer multiplication overflow\n");
    
    // Create two large numbers that would cause overflow
    bigint_t a, b, result;
    bigint_init(&a);
    bigint_init(&b);
    bigint_init(&result);
    
    // Fill with maximum values to test overflow
    for (int i = 0; i < 120; i++) {
        a.words[i] = 0xFFFFFFFF;
        b.words[i] = 0xFFFFFFFF;
    }
    a.used = 120;
    b.used = 120;
    
    printf("Attempting multiplication of %d-word by %d-word numbers\n", a.used, b.used);
    printf("Sum of used words: %d, max capacity: %d\n", a.used + b.used, BIGINT_4096_WORDS);
    
    int ret = bigint_mul(&result, &a, &b);
    printf("Multiplication result: %d\n", ret);
    
    if (ret == -2) {
        printf("ERROR: Multiplication overflow detected!\n");
        printf("This is the root cause of the Montgomery multiplication failure.\n");
    }
    
    return ret;
}