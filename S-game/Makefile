CC = gcc
TARGET = S-game
HDRS = json.hpp nanosvg.h nanosvgrast.h stb_image.h SDL3/SDL.h SDL3/SDL_main.h
VCPKG_ROOT = <C:\Users\sapph1ren\vcpkg>
VCPKG_INCLUDE = $(VCPKG_ROOT)\installed\x64-windows-static\include
VCPKG_LIB = $(VCPKG_ROOT)\installed\x64-windows-static\lib
CFLAGS = -Wall -Wextra -g -I. -I$(VCPKG_INCLUDE)
LDFLAGS = -L$(VCPKG_LIB) -lSDL3-static
all: $(TARGET)
$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
