CC = gcc

SRC_DIR = source
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

CFLAGS = -std=c17 -O2 -I./$(INCLUDE_DIR)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lm -lSDL2

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Executable name
EXECUTABLE = $(BIN_DIR)/vulkan_app

# Targets
all: $(BUILD_DIR) $(BIN_DIR) $(EXECUTABLE) shaders

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(EXECUTABLE) $(LDFLAGS) -rpath /usr/local/lib

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

shaders/%.spv: shaders/shader.%
	glslc ./shaders/shader.% -o ./shaders/%.spv

shaders: ./shaders/shader.vert ./shaders/shader.frag
	glslc ./shaders/shader.vert -o ./shaders/vert.spv
	glslc ./shaders/shader.frag -o ./shaders/frag.spv

test: all
	./$(EXECUTABLE)

.PHONY: all clean shaders test