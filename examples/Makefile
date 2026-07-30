CC ?= cc
PROJECT_ROOT ?= ..
BUILD_DIR ?= build

CPPFLAGS += -I$(PROJECT_ROOT)/include
CFLAGS ?= -std=c23 -Wall -Wextra -Wpedantic -g

# Adjust this list if your source directory uses different filenames.
LIBADT_SOURCES ?= \
	$(PROJECT_ROOT)/src/dynamic_array.c \
	$(PROJECT_ROOT)/src/comparators.c \
	$(PROJECT_ROOT)/src/printers.c

EXAMPLES := basic custom_type debug
BINARIES := $(addprefix $(BUILD_DIR)/,$(EXAMPLES))

.PHONY: all clean list \
	basic custom-type debug \
	run-basic run-custom-type run-debug

all: $(BINARIES)

list:
	@printf '%s\n' \
		'make run-basic       # primitive operations and normal printing' \
		'make run-custom-type # custom type info, ownership, and destruction' \
		'make run-debug       # normal output compared with DA_DEBUG output'

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/basic: basic.c $(LIBADT_SOURCES) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/custom_type: custom_type.c $(LIBADT_SOURCES) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/debug: debug.c $(LIBADT_SOURCES) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

basic: run-basic
custom-type: run-custom-type
debug: run-debug

run-basic: $(BUILD_DIR)/basic
	./$<

run-custom-type: $(BUILD_DIR)/custom_type
	./$<

run-debug: $(BUILD_DIR)/debug
	./$<

clean:
	rm -rf $(BUILD_DIR)
