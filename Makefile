# Copyright (C) 2023-2024  Lukasz Drzensla

PLATFORM ?= windows

CC =
CXX = 
APP = 

CFLAGS = -g -O0 -Wall -std=c17 -pedantic -Wunused -Werror -Wmissing-declarations
CCOPTS = -g -O0 -Wall -std=c++20

ifeq ($(PLATFORM), linux)
	CC = gcc
	APP = $(BUILD_DIR)/ppm
endif

ifeq ($(PLATFORM), windows)
	CC = clang
	APP = $(BUILD_DIR)/ppm.exe
	CFLAGS += -D_CRT_SECURE_NO_WARNINGS
endif

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

#main library objects, main.o to be moved
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/ppm.o $(BUILD_DIR)/hlp_str.o

#additional functionality
FILTERS ?= yes

ifeq ($(FILTERS), yes)
	OBJECTS += $(BUILD_DIR)/ppm_filters.o
endif

APP_OPT = -DDBG

LIB = $(BUILD_DIR)/LIB
LIB_OBJS = $(BUILD_DIR)/LIB.o
LIB_OPT =

INCLUDE = $(SRC_DIR)

default: $(BUILD_DIR) $(APP)
library: $(BUILD_DIR) $(LIB)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(OBJECTS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APP_OPT) -c $< -o $@

$(LIB_OBJS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CCOPTS) -I$(INCLUDE) $(LIB_OPT) -c $< -o $@

$(APP): $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APP_OPT) $(OBJECTS) -o $@

$(LIB): $(LIB_OBJS)
	$(CXX) $(CCOPTS) -I$(INCLUDE) $(LIB_OPT) $(LIB_OBJS) -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/ppm

