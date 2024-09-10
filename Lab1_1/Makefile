CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = sine_sum

ifeq ($(TYPE), double)
    CFLAGS += -DUSE_DOUBLE
endif

all: $(TARGET)

$(TARGET): main.c
 $(CC) $(CFLAGS) -o $(TARGET) main.c -lm

clean:
 rm -f $(TARGET)