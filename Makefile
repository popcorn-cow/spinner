# Build configuration. Override any of these with e.g. `make linux CC_LINUX=clang`.
APP          ?= spinner
BUILD_DIR    ?= bin
SOURCES      ?= $(wildcard src/*.c)
HEADERS      ?= $(wildcard inc/*.h)
CFLAGS       ?= -std=c99 -Wall -Wextra
RELEASE      ?= 0

ifeq ($(RELEASE),1)
MODE_CFLAGS  ?= -O2
MODE_SUFFIX  := -release
else
MODE_CFLAGS  ?= -g -O0
MODE_SUFFIX  :=
endif

# Windows (w64devkit + Raylib, matching the VS Code task).
CC_WINDOWS      ?= C:/raylib/w64devkit/bin/gcc.exe # change these if you have them in different directories
RAYLIB_WINDOWS  ?= C:/raylib/raylib/src
WINDOWS_CFLAGS  ?= $(CFLAGS)
WINDOWS_CPPFLAGS?= -I$(RAYLIB_WINDOWS) -I./inc
WINDOWS_LDFLAGS ?= -L$(RAYLIB_WINDOWS) -L./lib
WINDOWS_RELEASE_LDFLAGS ?= -Wl,--subsystem,windows
WINDOWS_LIBS    ?= -lraylib -lopengl32 -lgdi32 -lwinmm -lshcore -llua55

# Linux and macOS use a Raylib installation discoverable through pkg-config.
CC_LINUX       ?= gcc
LINUX_CFLAGS   ?= $(CFLAGS)
LINUX_CPPFLAGS ?= $(shell pkg-config --cflags raylib 2>/dev/null) -I./inc
LINUX_LDFLAGS  ?= $(shell pkg-config --libs-only-L raylib 2>/dev/null) -L./lib
LINUX_LIBS     ?= $(shell pkg-config --libs raylib 2>/dev/null) -lm -lpthread -lrt -lX11 -llua55

CC_MACOS       ?= clang
MACOS_CFLAGS   ?= $(CFLAGS)
MACOS_CPPFLAGS ?= $(shell pkg-config --cflags raylib 2>/dev/null) -I./inc
MACOS_LDFLAGS  ?= $(shell pkg-config --libs-only-L raylib 2>/dev/null) -L./lib
MACOS_LIBS     ?= $(shell pkg-config --libs raylib 2>/dev/null) -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -llua55

ifeq ($(OS),Windows_NT)
DEFAULT_PLATFORM := windows
else ifeq ($(shell uname -s 2>/dev/null),Darwin)
DEFAULT_PLATFORM := macos
else
DEFAULT_PLATFORM := linux
endif

.PHONY: all debug win windows tux linux loonix mac macos clean run install uninstall

# `make` builds for the operating system running make.
all: $(DEFAULT_PLATFORM)

# Debug is the default. Use RELEASE=1 for an optimized release executable.
debug: all

win windows:
	$(MAKE) --no-print-directory build TARGET=$(BUILD_DIR)/$(APP)$(MODE_SUFFIX).exe CC="$(CC_WINDOWS)" CPPFLAGS="$(WINDOWS_CPPFLAGS)" CFLAGS="$(WINDOWS_CFLAGS) $(MODE_CFLAGS)" LDFLAGS="$(WINDOWS_LDFLAGS) $(if $(filter 1,$(RELEASE)),$(WINDOWS_RELEASE_LDFLAGS))" LDLIBS="$(WINDOWS_LIBS)"

tux linux loonix:
	$(MAKE) --no-print-directory build TARGET=$(BUILD_DIR)/$(APP)-loonix$(MODE_SUFFIX).exe CC="$(CC_LINUX)" CPPFLAGS="$(LINUX_CPPFLAGS)" CFLAGS="$(LINUX_CFLAGS) $(MODE_CFLAGS)" LDFLAGS="$(LINUX_LDFLAGS)" LDLIBS="$(LINUX_LIBS)"

mac macos:
	$(MAKE) --no-print-directory build TARGET=$(BUILD_DIR)/$(APP)-macos$(MODE_SUFFIX) CC="$(CC_MACOS)" CPPFLAGS="$(MACOS_CPPFLAGS)" CFLAGS="$(MACOS_CFLAGS) $(MODE_CFLAGS)" LDFLAGS="$(MACOS_LDFLAGS)" LDLIBS="$(MACOS_LIBS)"

.PHONY: build
build:
	$(MAKE) --no-print-directory $(TARGET) CC="$(CC)" CPPFLAGS="$(CPPFLAGS)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" LDLIBS="$(LDLIBS)"

$(BUILD_DIR)/$(APP).exe: icon.o $(SOURCES) $(HEADERS)
# 	mkdir bin/
	windres icon.rc -O coff -o icon.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) icon.o -o "$@" $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/$(APP)-release.exe: $(SOURCES) $(HEADERS)
# 	mkdir bin/
	windres icon.rc -O coff -o icon.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) icon.o -o "$@" $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/$(APP)-loonix.exe: $(SOURCES) $(HEADERS)
	mkdir -p bin/
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o "$@" $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/$(APP)-loonix-release.exe: $(SOURCES) $(HEADERS)
	mkdir -p bin/
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o "$@" $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/$(APP)-macos: $(SOURCES) $(HEADERS)
	mkdir -p bin/
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o "$@" $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/$(APP)-macos-release: $(SOURCES) $(HEADERS)
	mkdir -p bin/
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o "$@" $(LDFLAGS) $(LDLIBS)

clean:
	ifeq ($(OS),Windows_NT)
		del /f $(BUILD_DIR)\*
	else
		rm -f $(BUILD_DIR)/*
	endif
run:
	$(BUILD_DIR)/$(APP)*$(MODE_SUFFIX).*
	
install:
	ifeq ($(OS), Windows_NT)
		echo "this operation is not supported on windows"
	else
		mv $(BUILD_DIR)/$(APP) /usr/bin/$(APP)
	endif

uninstall:
	ifeq ($(OS), Windows_NT)
		echo "this operation is not supported on windows"
	else
		rm -f /usr/bin/$(APP)
	endif
