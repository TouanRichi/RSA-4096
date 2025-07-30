/**
 * @file main.c
 * @brief Command-line interface for RSA-4096 with Montgomery REDC - Production Ready
 * 
 * @author TouanRichi
 * @date 2025-07-29 12:56:28 UTC
 * @version FINAL_COMPLETE_FIXED_v8.3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsa_4096.h"

int main(int argc, char **argv) {
    printf("[main:%d] Starting RSA-4096 application\n", __LINE__);
    if (argc < 2) {
        printf("Usage: %s [verify|test|benchmark|binary|manual|real4096|hybrid|roundtrip|boundary|montgomery|algorithms]\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "verify") == 0) {
        printf("[main:%d] Running verification mode (hex)\n", __LINE__);
        return run_verification();
    }
    if (strcmp(argv[1], "test") == 0) {
        printf("[main:%d] Running large RSA key testing\n", __LINE__);
        return test_large_rsa_keys();
    }
    if (strcmp(argv[1], "benchmark") == 0) {
        printf("[main:%d] Running performance benchmarks\n", __LINE__);
        return run_benchmarks();
    }
    if (strcmp(argv[1], "binary") == 0) {
        printf("[main:%d] Running binary operations verification\n", __LINE__);
        return run_binary_verification();
    }
    if (strcmp(argv[1], "manual") == 0) {
        printf("[main:%d] Running manual key input mode\n", __LINE__);
        return run_manual_key_test();
    }
    if (strcmp(argv[1], "real4096") == 0) {
        printf("[main:%d] Running real RSA-4096 key testing\n", __LINE__);
        return test_real_rsa_4096();
    }
    if (strcmp(argv[1], "hybrid") == 0) {
        printf("[main:%d] Running hybrid algorithm selection testing\n", __LINE__);
        return test_hybrid_algorithm_selection();
    }
    /* TODO: Enhanced round-trip testing commands */
    if (strcmp(argv[1], "roundtrip") == 0) {
        printf("[main:%d] Running comprehensive round-trip validation\n", __LINE__);
        return test_round_trip_comprehensive();
    }
    if (strcmp(argv[1], "boundary") == 0) {
        printf("[main:%d] Running boundary conditions testing\n", __LINE__);
        return test_boundary_conditions();
    }
    if (strcmp(argv[1], "montgomery") == 0) {
        printf("[main:%d] Running detailed Montgomery conversions testing\n", __LINE__);
        return test_montgomery_conversions_detailed();
    }
    if (strcmp(argv[1], "algorithms") == 0) {
        printf("[main:%d] Running algorithm consistency validation\n", __LINE__);
        return validate_all_algorithms_round_trip();
    }
    printf("Unknown command: %s\n", argv[1]);
    return 1;
}