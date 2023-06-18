# https://www.gnu.org/software/make/manual/html_node/Quick-Reference.html

CC = g++
CFLAGS = -std=c++20 -Wall -pedantic -Werror
LDFLAGS = -lm -lsfml-graphics -lsfml-window -lsfml-network -lsfml-system
INCLUDE = -I/usr/include -Isrc -Ilib/SFML/include

# Find all C++ source files and get their path seperated by a space,
# recursively.
SOURCE = $(shell find . -name "*.cpp" -printf "%P ")

# Object file names are source file names with a .o extension and are stored
# under obj instead of src.
OBJECTS = $(patsubst src/%, obj/%, $(patsubst %.cpp, %.o, $(SOURCE)))

# Folders are "bin" and the object folders, that are the source path directories
# by stored under obj and not src.
FOLDERS = bin obj $(patsubst src/%, obj/%, $(dir $(SOURCE)))

.PHONY: all debug folders
all: gameoflife.exe

# For debugging, add the debug flag and do target "all".
debug: CFLAGS += -g
debug: all

# Make recursive folder directory for the object files.
folders:
	mkdir -p $(FOLDERS)

# The executable depends on all object files, that are the source file names
# with a .o extension.
gameoflife.exe: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o bin/$@

# Each object file depends on it's source file.
obj/%.o: src/%.cpp
	$(CC) $< $(CFLAGS) $(INCLUDE) -c -o $(patsubst src/%, obj/%, $@)

clean:
	rm -rf obj bin
