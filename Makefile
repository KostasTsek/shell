# Compiler and flags
CC = gcc

# Output directory
OUT_DIR = out

# Source directories
SRC_DIRS = . executor AST tokenizer utils

# Find all source files
SRC = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJ = $(patsubst %.c,$(OUT_DIR)/%.o,$(SRC))

# Executable name
TARGET = $(OUT_DIR)/shell

# Default rule
all: $(TARGET)

# Ensure output directory structure exists
$(OUT_DIR):
	mkdir -p $(OUT_DIR)/executor $(OUT_DIR)/AST $(OUT_DIR)/tokenizer $(OUT_DIR)/utils

# Generic rule to compile .c -> .o into out/
$(OUT_DIR)/%.o: %.c | $(OUT_DIR)
	@mkdir -p $(dir $@)
	$(CC) -c $< -o $@

# Link everything into final binary
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)
	@echo "Build complete → $(TARGET)"

# Partial builds
tokenizer: $(OUT_DIR)/tokenizer/tokenizer.o
	@echo "Built tokenizer.o"

ast: $(OUT_DIR)/AST/ast.o
	@echo "Built ast.o"

executor: $(OUT_DIR)/executor/executor.o
	@echo "Built executor.o"

# Clean up
clean:
	rm -rf $(OUT_DIR)
