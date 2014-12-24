CFLAGS=-Wall -Wextra
LDLIBS=-ldl

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

all: test

test: $(TESTS)

.PHONY: clean
clean:
	rm  -rf $(TESTS)
