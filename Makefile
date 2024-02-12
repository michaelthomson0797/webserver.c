# ----- Variables for customization -----
CC = gcc  # Compiler (you can change to clang if desired)
CFLAGS = -g3 -Wall -Wextra -fsanitize=address,undefined # Standard flags + debugging & warnings
LDFLAGS = # Linker flags, if any

# ----- Automatic dependency tracking -----
SRCS := $(wildcard *.c)           # Find all .c source files
OBJS := $(SRCS:.c=.o)            # Corresponding object files (.o)
HDRS := $(SRCS:.c=.h)

# ----- Targets -----
server: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o server $(LDFLAGS)  

# Create object files from source files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Phony target for cleaning 
.PHONY: clean
clean:
	rm -f $(OBJS) server
