CC := gcc
CXX := g++

CFLAGS := -std=c23 -Wall -Wextra -Wpedantic -Werror -g
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -Werror -g

INCLUDES := -Iinclude

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests

TARGET := $(BUILD_DIR)/libcollections
TEST_TARGET := $(BUILD_DIR)/run_tests

C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

MAIN_OBJECT := $(BUILD_DIR)/main.o

TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
C_TEST_SOURCES := $(wildcard $(TEST_DIR)/*.c)
C_TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/tests/c_%.o,$(C_TEST_SOURCES))

CPPUTEST_INCLUDES := -I/usr/local/include
CPPUTEST_LIBS := -L/usr/local/lib -lCppUTest -lCppUTestExt


.PHONY: all build run test clean rebuild sanitize

all: build

build: $(TARGET)

$(TARGET): $(C_OBJECTS) $(MAIN_OBJECT)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/main.o: main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

run: build
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(C_OBJECTS) $(C_TEST_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(CPPUTEST_LIBS) -o $@

$(BUILD_DIR)/tests/c_%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)/tests
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CPPUTEST_INCLUDES) -c $< -o $@

sanitize: CFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: CXXFLAGS += -fsanitize=address,undefined -fno-omit-frame-pointer
sanitize: CPPUTEST_LIBS += -fsanitize=address,undefined
sanitize: clean test

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build
