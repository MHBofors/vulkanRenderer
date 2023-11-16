CC = gcc
INC_DIR = Include
SRC_DIR = Source
CFLAGS = -std=c17 -O2 -I./$(INC_DIR)
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lm



renderer: main.o
	$(CC) $(CFLAGS) main.o  -o renderer
	rm -f *.o

main.o: $(SRC_DIR)/main.c window.o renderer.o
	$(CC) $(CFLAGS) -c $< window.o renderer.o $(LDFLAGS)

window.o: $(SRC_DIR)/window.c $(INC_DIR)/window.h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

renderer.o: $(SRC_DIR)/renderer.c $(INC_DIR)/renderer.h vulkan_device.o vulkan_instance.o
	$(CC) $(CFLAGS) -c $< vulkan_device.o vulkan_instance.o $(LDFLAGS)

vulkan_device.o: $(SRC_DIR)/vulkan_device.c $(INC_DIR)/vulkan_device.h vulkan_utils.o
	$(CC) $(CFLAGS) -c $< vulkan_utils.o $(LDFLAGS)

vulkan_instance.o: $(SRC_DIR)/vulkan_instance.c $(INC_DIR)/vulkan_instance.h vulkan_utils.o
	$(CC) $(CFLAGS) -c $< vulkan_utils.o $(LDFLAGS)

vulkan_utils.o: $(SRC_DIR)/vulkan_utils.c $(INC_DIR)/vulkan_utils.h dynamic_vector.o
	$(CC) $(CFLAGS) -c $< dynamic_vector.o $(LDFLAGS)

dynamic_vector.o: $(SRC_DIR)/dynamic_vector.c $(INC_DIR)/dynamic_vector.h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS)

.PHONY: test clean

test: renderer
	./renderer

clean:
	rm -f renderer
	rm -f *.o
