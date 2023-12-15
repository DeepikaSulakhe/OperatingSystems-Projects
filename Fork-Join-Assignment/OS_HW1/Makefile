CC := gcc
CFLAGS := -Wall -g
TARGET := runnable

# $(wildcard *.c /xxx/xxx/*.c): get all .c files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard *.c)
# $(patsubst %.c,%.o,$(SRCS)): substitute all ".c" file name strings to ".o" file name strings
OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^
	rm -rf *.o
%.o: %.c
	$(CC) $(CFLAGS) -c $<
clean:
	rm -rf $(TARGET) *.o
	
.PHONY: all clean