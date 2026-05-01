# Makefile atualizado para automatizar OBJETOS e dependências
PROJ_NAME = ted
LIBS = -lm
TEST_SUPPORT_SRC = ./lib/unity/unity.c
# Tenta find primeiro, se falhar usa wildcard
SRC_FILES := $(shell find . -name "*.c" ! -name "t_*.c" ! -path "$(TEST_SUPPORT_SRC)" 2>/dev/null)
ifeq ($(SRC_FILES),)
    SRC_FILES := $(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
    SRC_FILES := $(filter-out %/t_%.c $(TEST_SUPPORT_SRC),$(SRC_FILES))
endif
OBJETOS := $(SRC_FILES:.c=.o)       # Substitui .c por .o

# Test files
TEST_FILES := $(shell find . -name "t_*.c" 2>/dev/null)
TEST_BINS := $(patsubst %.c,%_test,$(subst /t_,/,$(TEST_FILES)))

# All non-test, non-main, non-unity sources: each test is linked against all
# of these so that inter-module dependencies resolve without Makefile changes.
MAIN_SRC = ./main.c
TEST_LINK_SRCS := $(filter-out $(MAIN_SRC) $(TEST_SUPPORT_SRC),$(SRC_FILES))

# Compilador e Flags
CC = gcc
CFLAGS = -ggdb -O0 -std=c99 -fstack-protector-all -Werror=implicit-function-declaration
LDFLAGS = -O0

# Regra principal
$(PROJ_NAME): $(OBJETOS)
	$(CC) -o $(PROJ_NAME) $(LDFLAGS) $(OBJETOS) $(LIBS)

# Regra para compilar arquivos .c em .o com dependências
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# ============================================================================
# Test Targets
# ============================================================================

# Build and run all tests
test: test-build test-run

# Build all test executables (linked against all module sources for dep resolution)
test-build:
	@for test_src in $(TEST_FILES); do \
		test_bin=$$(dirname $$test_src)/$$(basename $$test_src .c | sed 's/^t_//')_test; \
		echo "Building test: $$test_bin"; \
		$(CC) $(CFLAGS) -o $$test_bin $$test_src $(TEST_LINK_SRCS) $(TEST_SUPPORT_SRC) $(LIBS) || exit 1; \
	done

# Run all tests
test-run: test-build
	@echo "========================================="
	@echo "Running All Tests"
	@echo "========================================="
	@for test in $(TEST_BINS); do \
		echo ""; \
		echo ">>> Running $$test"; \
		./$$test || exit 1; \
	done
	@echo ""
	@echo "========================================="
	@echo "All Tests Passed!"
	@echo "========================================="

# Clean test executables
test-clean:
	@echo "Cleaning test executables..."
	rm -f $(TEST_BINS)

# Target para limpeza
clean: test-clean
	rm -f $(OBJETOS) $(PROJ_NAME)

# Target para debug (mostra variáveis)
debug:
	@echo "SRC_FILES: $(SRC_FILES)"
	@echo "OBJETOS: $(OBJETOS)"
	@echo "TEST_FILES: $(TEST_FILES)"
	@echo "TEST_BINS: $(TEST_BINS)"
	@echo "TEST_SUPPORT_SRC: $(TEST_SUPPORT_SRC)"

run: $(PROJ_NAME)
	./$(PROJ_NAME) -f test/test.geo -o test/results -q test/test.qry

tstall: test

.PHONY: test test-build test-run test-clean clean debug run tstall
