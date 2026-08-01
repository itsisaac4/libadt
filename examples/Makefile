CC ?= cc
PROJECT_ROOT ?= ..
BUILD_DIR ?= build

CPPFLAGS += -I$(PROJECT_ROOT)/include
CFLAGS ?= -std=c23 -Wall -Wextra -Wpedantic -Werror -g

LIBADT_SOURCES ?= \
	$(PROJECT_ROOT)/src/shared/abstract_data_type.c \
	$(PROJECT_ROOT)/src/shared/statistics.c \
	$(PROJECT_ROOT)/src/shared/sorting.c \
	$(PROJECT_ROOT)/src/shared/element/comparators.c \
	$(PROJECT_ROOT)/src/shared/element/number_converters.c \
	$(PROJECT_ROOT)/src/shared/element/printers.c \
	$(PROJECT_ROOT)/src/shared/storage/contiguous_storage.c \
	$(PROJECT_ROOT)/src/shared/storage/linked_storage.c \
	$(PROJECT_ROOT)/src/containers/dynamic_array/dynamic_array.c \
	$(PROJECT_ROOT)/src/containers/dynamic_array/primitive_operations.c \
	$(PROJECT_ROOT)/src/containers/linked_list/linked_list.c \
	$(PROJECT_ROOT)/src/containers/linked_list/primitive_operations.c \
	$(PROJECT_ROOT)/src/containers/stack/stack.c \
	$(PROJECT_ROOT)/src/containers/stack/primitive_operations.c \
	$(PROJECT_ROOT)/src/containers/queue/queue.c \
	$(PROJECT_ROOT)/src/containers/queue/primitive_operations.c

COMMON_DIR := common
COMMON_BUILD_DIR := $(BUILD_DIR)/common
COMMON_EXAMPLES := polymorphism video_walkthrough
COMMON_BINARIES := $(addprefix $(COMMON_BUILD_DIR)/,$(COMMON_EXAMPLES))

DYNAMIC_ARRAY_DIR := dynamic_array
DYNAMIC_ARRAY_BUILD_DIR := $(BUILD_DIR)/dynamic_array
DYNAMIC_ARRAY_EXAMPLES := quick_start basic student_records debug_view
DYNAMIC_ARRAY_BINARIES := \
	$(addprefix $(DYNAMIC_ARRAY_BUILD_DIR)/,$(DYNAMIC_ARRAY_EXAMPLES))

LINKED_LIST_DIR := linked_list
LINKED_LIST_BUILD_DIR := $(BUILD_DIR)/linked_list
LINKED_LIST_EXAMPLES := quick_start basic owned_records
LINKED_LIST_BINARIES := \
	$(addprefix $(LINKED_LIST_BUILD_DIR)/,$(LINKED_LIST_EXAMPLES))

STACK_DIR := stack
STACK_BUILD_DIR := $(BUILD_DIR)/stack
STACK_EXAMPLES := quick_start basic
STACK_BINARIES := $(addprefix $(STACK_BUILD_DIR)/,$(STACK_EXAMPLES))

QUEUE_DIR := queue
QUEUE_BUILD_DIR := $(BUILD_DIR)/queue
QUEUE_EXAMPLES := quick_start basic
QUEUE_BINARIES := $(addprefix $(QUEUE_BUILD_DIR)/,$(QUEUE_EXAMPLES))

SORTING_DIR := sorting
SORTING_BUILD_DIR := $(BUILD_DIR)/sorting
SORTING_EXAMPLES := algorithms custom_order
SORTING_BINARIES := \
	$(addprefix $(SORTING_BUILD_DIR)/,$(SORTING_EXAMPLES))

STATISTICS_DIR := statistics
STATISTICS_BUILD_DIR := $(BUILD_DIR)/statistics
STATISTICS_EXAMPLES := summary
STATISTICS_BINARIES := \
	$(addprefix $(STATISTICS_BUILD_DIR)/,$(STATISTICS_EXAMPLES))

.PHONY: all clean list polymorphism video-walkthrough dynamic-array student-records debug-view \
	dynamic-array-basic linked-list owned-records linked-list-basic stack \
	stack-basic queue queue-basic sorting custom-order statistics

all: $(COMMON_BINARIES) $(DYNAMIC_ARRAY_BINARIES) $(LINKED_LIST_BINARIES) \
	$(STACK_BINARIES) $(QUEUE_BINARIES) $(SORTING_BINARIES) $(STATISTICS_BINARIES)

list:
	@printf '%s\n' \
		'make polymorphism   # one shared API over every container' \
		'make video-walkthrough # presentation-friendly tour of course concepts' \
		'make dynamic-array  # primitive dynamic-array operations' \
		'make dynamic-array-basic # unchecked array operation walkthrough' \
		'make student-records # custom dynamic-array callbacks' \
		'make debug-view     # shared debug information' \
		'make linked-list    # primitive linked-list operations' \
		'make linked-list-basic # unchecked list operation walkthrough' \
		'make owned-records  # resource ownership and transfer' \
		'make stack          # contiguous LIFO operations' \
		'make stack-basic    # unchecked stack operation walkthrough' \
		'make queue          # linked FIFO operations' \
		'make queue-basic    # unchecked queue operation walkthrough' \
		'make sorting        # every sorting algorithm' \
		'make custom-order   # per-call comparator overrides' \
		'make statistics     # polymorphic numeric statistics'

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

$(STACK_BUILD_DIR):
	mkdir -p $@

$(STACK_BUILD_DIR)/%: \
	$(STACK_DIR)/%.c \
	$(LIBADT_SOURCES) | $(STACK_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(QUEUE_BUILD_DIR):
	mkdir -p $@

$(QUEUE_BUILD_DIR)/%: \
	$(QUEUE_DIR)/%.c \
	$(LIBADT_SOURCES) | $(QUEUE_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(SORTING_BUILD_DIR):
	mkdir -p $@

$(SORTING_BUILD_DIR)/%: \
	$(SORTING_DIR)/%.c \
	$(LIBADT_SOURCES) | $(SORTING_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(STATISTICS_BUILD_DIR):
	mkdir -p $@

$(STATISTICS_BUILD_DIR)/%: \
	$(STATISTICS_DIR)/%.c \
	$(LIBADT_SOURCES) | $(STATISTICS_BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

polymorphism: $(COMMON_BUILD_DIR)/polymorphism
	./$<

video-walkthrough: $(COMMON_BUILD_DIR)/video_walkthrough
	./$<

dynamic-array: $(DYNAMIC_ARRAY_BUILD_DIR)/quick_start
	./$<

dynamic-array-basic: $(DYNAMIC_ARRAY_BUILD_DIR)/basic
	./$<

student-records: $(DYNAMIC_ARRAY_BUILD_DIR)/student_records
	./$<

debug-view: $(DYNAMIC_ARRAY_BUILD_DIR)/debug_view
	./$<

linked-list: $(LINKED_LIST_BUILD_DIR)/quick_start
	./$<

linked-list-basic: $(LINKED_LIST_BUILD_DIR)/basic
	./$<

owned-records: $(LINKED_LIST_BUILD_DIR)/owned_records
	./$<

stack: $(STACK_BUILD_DIR)/quick_start
	./$<

stack-basic: $(STACK_BUILD_DIR)/basic
	./$<

queue: $(QUEUE_BUILD_DIR)/quick_start
	./$<

queue-basic: $(QUEUE_BUILD_DIR)/basic
	./$<

sorting: $(SORTING_BUILD_DIR)/algorithms
	./$<

custom-order: $(SORTING_BUILD_DIR)/custom_order
	./$<

statistics: $(STATISTICS_BUILD_DIR)/summary
	./$<

clean:
	rm -rf $(BUILD_DIR)
