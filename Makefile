# Makefile

# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall $(shell sdl2-config --cflags)

# Debug flags
ifdef DEBUG
CFLAGS += -DDEBUG
endif

# Source files
SRC = $(wildcard src/*.c)

# Object files
OBJ = $(SRC:src/%.c=src/%.o)

# Executable name
EXEC = tanhell

# Default target
all: $(EXEC)

# Linking the executable from the object files
$(EXEC): $(OBJ)
	@echo ""
	@echo "- Linking"
	$(CC) $(OBJ) -o $(EXEC) $(shell sdl2-config --libs) $(shell pkg-config --cflags --libs x11) -lm
	@echo ""
	@echo "- Cleanup"
	rm -f $(OBJ)
	@echo ""
	@echo "- Success"
	@echo "executable: $(EXEC)"
	
# Compiling the source files into object files
$(info - Compiling)
%.o: src/%.c
	$(CC) $(CFLAGS) -c $<

# Clean target
clean:
	rm -f $(EXEC) $(OBJ)

# Debug target
debug: 
	$(MAKE) DEBUG=1

# Phony targets
.PHONY: all clean debug
