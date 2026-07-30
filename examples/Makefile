CC ?= cc
PROJECT_ROOT ?= ..
BUILD_DIR ?= build

CPPFLAGS += -I$(PROJECT_ROOT)/include
CFLAGS ?= -std=c23 -Wall -Wextra -Wpedantic -Werror -g

LIBADT_SOURCES ?= \
	$(PROJECT_ROOT)/src/abstract_data_type.c \
	$(PROJECT_ROOT)/src/statistics.c \
	$(PROJECT_ROOT)/src/sorting.c \
	$(PROJECT_ROOT)/src/dynamic_array.c \
	$(PROJECT_ROOT)/src/linked_list.c \
	$(PROJECT_ROOT)/src/comparators.c \
	$(PROJECT_ROOT)/src/printers.c

COMMON_DIR := common
COMMON_BUILD_DIR := $(BUILD_DIR)/common
COMMON_EXAMPLES := polymorphism
COMMON_BINARIES := $(addprefix $(COMMON_BUILD_DIR)/,$(COMMON_EXAMPLES))

DYNAMIC_ARRAY_DIR := dynamic_array
DYNAMIC_ARRAY_BUILD_DIR := $(BUILD_DIR)/dynamic_array
DYNAMIC_ARRAY_EXAMPLES := quick_start student_records debug_view
DYNAMIC_ARRAY_BINARIES := \
	$(addprefix $(DYNAMIC_ARRAY_BUILD_DIR)/,$(DYNAMIC_ARRAY_EXAMPLES))

LINKED_LIST_DIR := linked_list
LINKED_LIST_BUILD_DIR := $(BUILD_DIR)/linked_list
LINKED_LIST_EXAMPLES := quick_start owned_records
LINKED_LIST_BINARIES := \
	$(addprefix $(LINKED_LIST_BUILD_DIR)/,$(LINKED_LIST_EXAMPLES))

SORTING_DIR := sorting
SORTING_BUILD_DIR := $(BUILD_DIR)/sorting
SORTING_EXAMPLES := algorithms custom_order
SORTING_BINARIES := \
	$(addprefix $(SORTING_BUILD_DIR)/,$(SORTING_EXAMPLES))

.PHONY: all clean list polymorphism dynamic-array student-records debug-view \
	linked-list owned-records sorting custom-order

all: $(COMMON_BINARIES) $(DYNAMIC_ARRAY_BINARIES) $(LINKED_LIST_BINARIES) $(SORTING_BINARIES)

list:
	@printf '%s\n' \
		'make polymorphism   # one shared API over arrays and lists' \
		'make dynamic-array  # primitive dynamic-array operations' \
		'make student-records # custom dynamic-array callbacks' \
		'make debug-view     # shared debug information' \
		'make linked-list    # primitive linked-list operations' \
		'make owned-records  # resource ownership and transfer' \
		'make sorting        # every sorting algorithm' \
		'make custom-order   # per-call comparator overrides'

$(COMMON_BUILD_DIR):
	mkdir -p $@

$(COMMON_BUILD_DIR)/%: \
	$(COMMON_DIR)/%.c \
	$(LIBADT_SOURCES) | $(COMMON_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(DYNAMIC_ARRAY_BUILD_DIR):
	mkdir -p $@

$(DYNAMIC_ARRAY_BUILD_DIR)/%: \
	$(DYNAMIC_ARRAY_DIR)/%.c \
	$(LIBADT_SOURCES) | $(DYNAMIC_ARRAY_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(LINKED_LIST_BUILD_DIR):
	mkdir -p $@

$(LINKED_LIST_BUILD_DIR)/%: \
	$(LINKED_LIST_DIR)/%.c \
	$(LIBADT_SOURCES) | $(LINKED_LIST_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(SORTING_BUILD_DIR):
	mkdir -p $@

$(SORTING_BUILD_DIR)/%: \
	$(SORTING_DIR)/%.c \
	$(LIBADT_SOURCES) | $(SORTING_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

polymorphism: $(COMMON_BUILD_DIR)/polymorphism
	./$<

dynamic-array: $(DYNAMIC_ARRAY_BUILD_DIR)/quick_start
	./$<

student-records: $(DYNAMIC_ARRAY_BUILD_DIR)/student_records
	./$<

debug-view: $(DYNAMIC_ARRAY_BUILD_DIR)/debug_view
	./$<

linked-list: $(LINKED_LIST_BUILD_DIR)/quick_start
	./$<

owned-records: $(LINKED_LIST_BUILD_DIR)/owned_records
	./$<

sorting: $(SORTING_BUILD_DIR)/algorithms
	./$<

custom-order: $(SORTING_BUILD_DIR)/custom_order
	./$<

clean:
	rm -rf $(BUILD_DIR)
