# CKOS Unit Tests Makefile
# Builds and runs unit tests for UI components and application logic integration

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O0 -DUNIT_TEST -DSIMULATOR
INCLUDES = -I../../App -I../../App/BSP
LDFLAGS = 

# Test source files
TEST_SOURCES = test_display_ui.c test_app_ui_integration.c

# Application source files needed for testing (select components)
APP_SOURCES = ../../App/Display/display_api.c \
              ../../App/AppLogic/app_logic.c \
              ../../App/Utils/utils.c

# Test executables
TEST_DISPLAY_UI = test_display_ui
TEST_APP_UI_INTEGRATION = test_app_ui_integration

# Object directories
OBJ_DIR = obj
BIN_DIR = bin

# All tests
ALL_TESTS = $(TEST_DISPLAY_UI) $(TEST_APP_UI_INTEGRATION)

.PHONY: all clean run run-display run-integration help

# Default target
all: $(ALL_TESTS)

# Create directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build display UI tests
$(TEST_DISPLAY_UI): test_display_ui.c | $(BIN_DIR)
	@echo "Building Display UI Tests..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_DIR)/$@ $< ../../App/Display/display_api.c $(LDFLAGS)
	@echo "Display UI Tests built successfully"

# Build app UI integration tests  
$(TEST_APP_UI_INTEGRATION): test_app_ui_integration.c | $(BIN_DIR)
	@echo "Building App UI Integration Tests..."
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_DIR)/$@ $< ../../App/AppLogic/app_logic.c $(LDFLAGS)
	@echo "App UI Integration Tests built successfully"

# Run all tests
run: all
	@echo "Running All Unit Tests"
	@echo "======================"
	@echo ""
	@echo "1. Display UI Component Tests:"
	@./$(BIN_DIR)/$(TEST_DISPLAY_UI)
	@echo ""
	@echo "2. App UI Integration Tests:"
	@./$(BIN_DIR)/$(TEST_APP_UI_INTEGRATION)
	@echo ""
	@echo "All tests completed!"

# Run individual test suites
run-display: $(TEST_DISPLAY_UI)
	@echo "Running Display UI Tests..."
	@./$(BIN_DIR)/$(TEST_DISPLAY_UI)

run-integration: $(TEST_APP_UI_INTEGRATION)
	@echo "Running App UI Integration Tests..."
	@./$(BIN_DIR)/$(TEST_APP_UI_INTEGRATION)

# Coverage analysis (if gcov is available)
coverage: CFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: all
	@echo "Running tests with coverage analysis..."
	@./$(BIN_DIR)/$(TEST_DISPLAY_UI) || true
	@./$(BIN_DIR)/$(TEST_APP_UI_INTEGRATION) || true
	@echo "Generating coverage report..."
	@gcov *.gcda 2>/dev/null || echo "gcov not available or no coverage data"

# Valgrind memory testing (if valgrind is available)
valgrind: all
	@echo "Running tests with Valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(TEST_DISPLAY_UI) || echo "Valgrind not available"
	@valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/$(TEST_APP_UI_INTEGRATION) || echo "Valgrind not available"

# Static analysis (if cppcheck is available)
static-analysis:
	@echo "Running static analysis..."
	@cppcheck --enable=all --std=c99 $(TEST_SOURCES) 2>/dev/null || echo "cppcheck not available"

# Clean build artifacts
clean:
	@echo "Cleaning test artifacts..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@rm -f *.gcov *.gcda *.gcno
	@echo "Clean complete"

# Performance benchmark
benchmark: all
	@echo "Running performance benchmarks..."
	@echo "Display UI Tests Performance:"
	@time ./$(BIN_DIR)/$(TEST_DISPLAY_UI) > /dev/null
	@echo "App UI Integration Tests Performance:"
	@time ./$(BIN_DIR)/$(TEST_APP_UI_INTEGRATION) > /dev/null

# Stress testing
stress: all
	@echo "Running stress tests (multiple iterations)..."
	@for i in $$(seq 1 10); do \
		echo "Iteration $$i:"; \
		./$(BIN_DIR)/$(TEST_DISPLAY_UI) > /dev/null && echo "  Display UI: PASS" || echo "  Display UI: FAIL"; \
		./$(BIN_DIR)/$(TEST_APP_UI_INTEGRATION) > /dev/null && echo "  Integration: PASS" || echo "  Integration: FAIL"; \
	done

# Help target
help:
	@echo "CKOS Unit Tests Makefile"
	@echo "========================"
	@echo ""
	@echo "Targets:"
	@echo "  all              Build all test executables"
	@echo "  run              Run all tests"
	@echo "  run-display      Run display UI tests only"
	@echo "  run-integration  Run app UI integration tests only"
	@echo "  coverage         Run tests with coverage analysis"
	@echo "  valgrind         Run tests with memory leak detection"
	@echo "  static-analysis  Run static code analysis"
	@echo "  benchmark        Run performance benchmarks"
	@echo "  stress           Run stress tests (10 iterations)"
	@echo "  clean            Remove all build artifacts"
	@echo "  help             Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make             # Build all tests"
	@echo "  make run         # Build and run all tests"
	@echo "  make coverage    # Run with coverage analysis"
	@echo "  make clean       # Clean up build files"

# Test information
info:
	@echo "Test Configuration:"
	@echo "  Compiler: $(CC)"
	@echo "  Flags: $(CFLAGS)"
	@echo "  Includes: $(INCLUDES)"
	@echo "  Tests: $(ALL_TESTS)"
	@echo "  Sources: $(TEST_SOURCES)"