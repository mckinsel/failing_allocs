CFLAGS=-Wall -Wextra
LDLIBS=-ldl

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

EXAMPLE_SRC=$(wildcard examples/*.c)
EXAMPLES=$(patsubst %.c,%,$(EXAMPLE_SRC))

all: examples tests

tests: $(TESTS)

examples: $(EXAMPLES)

.PHONY: clean
clean:
	rm  -rf $(TESTS)
	rm  -rf $(EXAMPLES)
