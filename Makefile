CC := gcc
CXX := g++

CFLAGS := -std=c23 -Wall -Wextra -Wpedantic -Werror -g
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -Werror -g
DEPFLAGS := -MMD -MP
SANITIZE_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer

INCLUDES := -Iinclude

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests
SRC_DIRS := \
	$(SRC_DIR)/shared \
	$(SRC_DIR)/shared/element \
	$(SRC_DIR)/shared/storage \
	$(SRC_DIR)/containers/dynamic_array \
	$(SRC_DIR)/containers/linked_list \
	$(SRC_DIR)/containers/stack \
	$(SRC_DIR)/containers/queue

TARGET := $(BUILD_DIR)/libcollections
TEST_TARGET := $(BUILD_DIR)/run_tests

C_SOURCES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))

MAIN_OBJECT := $(BUILD_DIR)/main.o

TEST_SOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/tests/%.o,$(TEST_SOURCES))
C_TEST_SOURCES := $(wildcard $(TEST_DIR)/*.c)
C_TEST_OBJECTS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/tests/c_%.o,$(C_TEST_SOURCES))
DEPENDENCIES := $(C_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) \
	$(TEST_OBJECTS:.o=.d) $(C_TEST_OBJECTS:.o=.d)

CPPUTEST_INCLUDES := -I/usr/local/include
CPPUTEST_LIBS := -L/usr/local/lib -lCppUTest -lCppUTestExt


.PHONY: all build run test clean rebuild sanitize

all: build

build: $(TARGET)

$(TARGET): $(C_OBJECTS) $(MAIN_OBJECT)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/main.o: main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

run: build
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(C_OBJECTS) $(C_TEST_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(CPPUTEST_LIBS) -o $@

$(BUILD_DIR)/tests/c_%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)/tests
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)/tests
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) $(CPPUTEST_INCLUDES) -c $< -o $@

sanitize:
	$(MAKE) BUILD_DIR=$(BUILD_DIR)/sanitize \
		CFLAGS="$(CFLAGS) $(SANITIZE_FLAGS)" \
		CXXFLAGS="$(CXXFLAGS) $(SANITIZE_FLAGS)" \
		CPPUTEST_LIBS="$(CPPUTEST_LIBS) $(SANITIZE_FLAGS)" test

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean build

-include $(DEPENDENCIES)
