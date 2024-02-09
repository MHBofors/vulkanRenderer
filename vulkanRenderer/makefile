CC = gcc
INC_DIR = include
SRC_DIR = source
BUILD_DIR = build
CFLAGS = -std=c17 -O2 -I./$(INC_DIR)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lm
OBJ_NAMES = main.o window.o renderer.o vulkan_resources.o vulkan_device.o vulkan_swap_chain.o vulkan_instance.o vulkan_debug.o vulkan_shader.o vulkan_utils.o dynamic_vector.o tree.o
OBJS = $(addprefix $(BUILD_DIR)/, $(OBJ_NAMES))

$(BUILD_DIR)/renderer: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_DIR)/renderer $(LDFLAGS) -rpath /usr/local/lib

$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c $(BUILD_DIR)/window.o $(BUILD_DIR)/renderer.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/window.o: $(SRC_DIR)/window.c $(INC_DIR)/window.h $(BUILD_DIR)/dynamic_vector.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/renderer.o: $(SRC_DIR)/renderer.c $(INC_DIR)/renderer.h $(BUILD_DIR)/vulkan_instance.o $(BUILD_DIR)/vulkan_device.o $(BUILD_DIR)/vulkan_swap_chain.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_shader.o: $(SRC_DIR)/vulkan_shader.c $(INC_DIR)/vulkan_shader.h $(BUILD_DIR)/vulkan_utils.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_resources.o: $(SRC_DIR)/vulkan_resources.c $(INC_DIR)/vulkan_resources.h $(BUILD_DIR)/vulkan_utils.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_swap_chain.o: $(SRC_DIR)/vulkan_swap_chain.c $(INC_DIR)/vulkan_swap_chain.h $(BUILD_DIR)/vulkan_utils.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_device.o: $(SRC_DIR)/vulkan_device.c $(INC_DIR)/vulkan_device.h $(BUILD_DIR)/vulkan_utils.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_instance.o: $(SRC_DIR)/vulkan_instance.c $(INC_DIR)/vulkan_instance.h $(BUILD_DIR)/vulkan_utils.o $(BUILD_DIR)/vulkan_debug.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_debug.o: $(SRC_DIR)/vulkan_debug.c $(INC_DIR)/vulkan_debug.h $(BUILD_DIR)/vulkan_utils.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/vulkan_utils.o: $(SRC_DIR)/vulkan_utils.c $(INC_DIR)/vulkan_utils.h $(BUILD_DIR)/dynamic_vector.o $(BUILD_DIR)/tree.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/tree.o: $(SRC_DIR)/tree.c $(INC_DIR)/tree.h $(INC_DIR)/dynamic_vector.h $(BUILD_DIR)/dynamic_vector.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/dynamic_vector.o: $(SRC_DIR)/dynamic_vector.c $(INC_DIR)/dynamic_vector.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: test clean obj

test: $(BUILD_DIR)/renderer
	./$(BUILD_DIR)/renderer

clean:
	rm -f $(BUILD_DIR)/renderer
	rm -f $(BUILD_DIR)/*.o