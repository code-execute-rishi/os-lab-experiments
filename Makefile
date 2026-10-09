CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = os_labs_5_and_6
SRC = os_labs_5_and_6.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET) --all

.PHONY: all clean run
