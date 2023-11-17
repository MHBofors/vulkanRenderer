CC = gcc
INC_DIR = Include
SRC_DIR = Source
CFLAGS = -std=c17 -O2 -I./$(INC_DIR)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lm
OBJS = main.o window.o renderer.o vulkan_device.o vulkan_instance.o vulkan_debug.o vulkan_utils.o dynamic_vector.o

renderer: main.o window.o renderer.o vulkan_device.o vulkan_instance.o vulkan_debug.o vulkan_utils.o dynamic_vector.o
	$(CC) $(CFLAGS) $(OBJS) -o renderer $(LDFLAGS)

main.o: $(SRC_DIR)/main.c $(INC_DIR)/window.h $(INC_DIR)/renderer.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

window.o: $(SRC_DIR)/window.c $(INC_DIR)/window.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

renderer.o: $(SRC_DIR)/renderer.c $(INC_DIR)/renderer.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

vulkan_device.o: $(SRC_DIR)/vulkan_device.c $(INC_DIR)/vulkan_device.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

vulkan_instance.o: $(SRC_DIR)/vulkan_instance.c $(INC_DIR)/vulkan_instance.h vulkan_debug.o
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

vulkan_debug.o: $(SRC_DIR)/vulkan_debug.c $(INC_DIR)/vulkan_debug.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

vulkan_utils.o: $(SRC_DIR)/vulkan_utils.c $(INC_DIR)/vulkan_utils.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

dynamic_vector.o: $(SRC_DIR)/dynamic_vector.c $(INC_DIR)/dynamic_vector.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

.PHONY: test clean

test: renderer
	./renderer

clean:
	rm -f renderer
	rm -f *.o
