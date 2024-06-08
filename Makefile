CC = gcc
CFLAGS = -pthread -Wall -Wextra -O2
TARGET = task
SRCS = main.c read_input.c write_output.c cJSON.c
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
