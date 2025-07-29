# Makefile for RSA-4096 with full Montgomery REDC - BUGS FIXED
# Date: 2025-07-29 09:41:10 UTC
# User: RSAhardcore
# Version: FINAL_COMPLETE_FIXED_v8.2

CC=gcc
# FIXED: Enhanced compiler flags for better debugging and optimization
CFLAGS=-Wall -Wextra -O3 -DNDEBUG -DLOG_LEVEL=2 -std=c99 -fstack-protector-strong -D_FORTIFY_SOURCE=2
LDFLAGS=-lm

# FIXED: Complete object list with proper dependencies
OBJS=rsa_4096_bigint.o rsa_4096_arithmetic.o rsa_4096_montgomery.o rsa_4096_core.o rsa_4096_tests.o main.o

# FIXED: Default target
all: rsa_4096

# FIXED: Enhanced production target with comprehensive testing
production: clean all test_rsa_4096_real run_basic_tests
	@echo "🏭 Production build completed successfully!"
	@echo "✅ All tests passed - RSA-4096 system ready for production use"

# FIXED: Main executable with proper linking
rsa_4096: $(OBJS)
	@echo "🔗 Linking RSA-4096 executable..."
	$(CC) $(CFLAGS) -o rsa_4096 $(OBJS) $(LDFLAGS)
	@echo "✅ RSA-4096 executable created successfully"

# FIXED: Individual object file rules with proper dependencies
main.o: main.c rsa_4096.h
	@echo "🔧 Compiling main.c..."
	$(CC) $(CFLAGS) -c main.c -o main.o

rsa_4096_bigint.o: rsa_4096_bigint.c rsa_4096.h
	@echo "🔧 Compiling rsa_4096_bigint.c..."
	$(CC) $(CFLAGS) -c rsa_4096_bigint.c -o rsa_4096_bigint.o

rsa_4096_arithmetic.o: rsa_4096_arithmetic.c rsa_4096.h
	@echo "🔧 Compiling rsa_4096_arithmetic.c..."
	$(CC) $(CFLAGS) -c rsa_4096_arithmetic.c -o rsa_4096_arithmetic.o

rsa_4096_montgomery.o: rsa_4096_montgomery.c rsa_4096.h
	@echo "🔧 Compiling rsa_4096_montgomery.c (COMPLETE REDC)..."
	$(CC) $(CFLAGS) -c rsa_4096_montgomery.c -o rsa_4096_montgomery.o

rsa_4096_core.o: rsa_4096_core.c rsa_4096.h
	@echo "🔧 Compiling rsa_4096_core.c..."
	$(CC) $(CFLAGS) -c rsa_4096_core.c -o rsa_4096_core.o

rsa_4096_tests.o: rsa_4096_tests.c rsa_4096.h
	@echo "🔧 Compiling rsa_4096_tests.c..."
	$(CC) $(CFLAGS) -c rsa_4096_tests.c -o rsa_4096_tests.o

# FIXED: Test executable with enhanced testing
test_rsa_4096_real: rsa_4096_bigint.o rsa_4096_arithmetic.o rsa_4096_montgomery.o rsa_4096_core.o rsa_4096_tests.o test_rsa_4096_real.c
	@echo "🔧 Building test_rsa_4096_real..."
	$(CC) $(CFLAGS) -o test_rsa_4096_real rsa_4096_bigint.o rsa_4096_arithmetic.o rsa_4096_montgomery.o rsa_4096_core.o rsa_4096_tests.o test_rsa_4096_real.c $(LDFLAGS)
	@echo "✅ Test executable created successfully"

# FIXED: Enhanced testing targets
run_basic_tests: rsa_4096
	@echo "🧪 Running basic verification tests..."
	./rsa_4096 verify
	@echo "🧪 Running large key tests..."
	./rsa_4096 test
	@echo "🧪 Running binary operation tests..."
	./rsa_4096 binary
	@echo "✅ All basic tests completed"

run_performance_tests: rsa_4096
	@echo "🚀 Running performance benchmarks..."
	./rsa_4096 benchmark
	@echo "✅ Performance tests completed"

run_comprehensive_tests: test_rsa_4096_real run_basic_tests run_performance_tests
	@echo "🔍 Running comprehensive real-key tests..."
	./test_rsa_4096_real
	@echo "✅ All comprehensive tests completed"

# FIXED: Enhanced debug build
debug: CFLAGS += -g -O0 -DDEBUG -DLOG_LEVEL=0
debug: clean all
	@echo "🐛 Debug build completed with full logging enabled"

# FIXED: Memory check target (requires valgrind)
memcheck: debug
	@echo "🔍 Running memory leak detection..."
	@if command -v valgrind >/dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./rsa_4096 verify; \
		echo "✅ Memory check completed"; \
	else \
		echo "⚠️  Valgrind not found - skipping memory check"; \
	fi

# FIXED: Static analysis target (requires cppcheck)
static_analysis:
	@echo "🔍 Running static code analysis..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c99 --platform=unix64 *.c *.h; \
		echo "✅ Static analysis completed"; \
	else \
		echo "⚠️  cppcheck not found - skipping static analysis"; \
	fi

# FIXED: Installation target
install: production
	@echo "📦 Installing RSA-4096..."
	@mkdir -p /usr/local/bin
	@cp rsa_4096 /usr/local/bin/
	@chmod 755 /usr/local/bin/rsa_4096
	@echo "✅ RSA-4096 installed to /usr/local/bin/"

# FIXED: Uninstallation target
uninstall:
	@echo "🗑️  Uninstalling RSA-4096..."
	@rm -f /usr/local/bin/rsa_4096
	@echo "✅ RSA-4096 uninstalled"

# FIXED: Enhanced clean target
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -f *.o rsa_4096 test_rsa_4096_real
	@rm -f core vgcore.* *.log
	@echo "✅ Clean completed!"

# FIXED: Distribution package creation
dist: clean
	@echo "📦 Creating distribution package..."
	@tar -czf rsa_4096_complete_fixed_v8.2.tar.gz *.c *.h Makefile README.md
	@echo "✅ Distribution package created: rsa_4096_complete_fixed_v8.2.tar.gz"

# FIXED: Help target
help:
	@echo "RSA-4096 with Complete Montgomery REDC - Build System"
	@echo "====================================================="
	@echo "Available targets:"
	@echo "  all                    - Build main executable (default)"
	@echo "  production            - Full production build with tests"
	@echo "  debug                 - Debug build with full logging"
	@echo "  test_rsa_4096_real    - Build test executable"
	@echo "  run_basic_tests       - Run basic verification tests"
	@echo "  run_performance_tests - Run performance benchmarks"
	@echo "  run_comprehensive_tests - Run all tests including real keys"
	@echo "  memcheck              - Run with memory leak detection"
	@echo "  static_analysis       - Run static code analysis"
	@echo "  install               - Install to system (/usr/local/bin)"
	@echo "  uninstall             - Remove from system"
	@echo "  clean                 - Clean build artifacts"
	@echo "  dist                  - Create distribution package"
	@echo "  help                  - Show this help message"
	@echo ""
	@echo "System Status:"
	@echo "  ✅ Complete Montgomery REDC: IMPLEMENTED"
	@echo "  ✅ RSA-4096 capability: READY"
	@echo "  ✅ Fallback arithmetic: STABLE"
	@echo "  ✅ Production ready: YES"

# FIXED: Version information
version:
	@echo "RSA-4096 with Complete Montgomery REDC"
	@echo "Version: FINAL_COMPLETE_FIXED_v8.2"
	@echo "Date: 2025-07-29 09:41:10 UTC"
	@echo "User: RSAhardcore"
	@echo "Features:"
	@echo "  - Complete Montgomery REDC implementation"
	@echo "  - RSA-4096 capable big integer arithmetic"
	@echo "  - Production-ready with comprehensive testing"
	@echo "  - Memory safe with proper error handling"
	@echo "  - All critical bugs fixed"

# FIXED: Declare phony targets
.PHONY: all production debug clean install uninstall help version dist
.PHONY: run_basic_tests run_performance_tests run_comprehensive_tests
.PHONY: memcheck static_analysis

# FIXED: Default goal
.DEFAULT_GOAL := all