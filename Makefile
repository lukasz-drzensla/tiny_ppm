PLATFORM = windows

CC =
CXX = 

CFLAGS = -g -Wall -std=c17 -pedantic -Wunused
CCOPTS = -g -Wall -std=c++20

ifeq ($(PLATFORM), linux)
	$(CC) = gcc
endif

ifeq ($(PLATFORM), windows)
	CC = gcc
endif

SRC_DIR = ./src
BUILD_DIR = ./build
LIB_DIR = ./lib

APP = $(BUILD_DIR)/ppm.exe
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/ppm.o
APT_OPT = -DDBG

LIB = $(BUILD_DIR)/LIB
LIB_OBJS = $(BUILD_DIR)/LIB.o
LIB_OPT =

INCLUDE = $(SRC_DIR)

default: $(APP)

$(OBJECTS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APT_OPT) -c $< -o $@

$(LIB_OBJS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CPP) $(CCOPTS) -I$(INCLUDE) $(LIB_OPT) -c $< -o $@

$(APP): $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APT_OPT) $(OBJECTS) -o $@

$(LIB): $(LIB_OBJS)
	$(CPP) $(CCOPTS) -I$(INCLUDE) $(LIB_OPT) $(LIB_OBJS) -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/LIB
	rm $(BUILD_DIR)/bench

