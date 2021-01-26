##
# MultiWC
#
# @file
# @version 0.1

.PHONY: bin
bin: mwc

%.c: %.h

mwc: counter.c main.c
	gcc -O3 -o $@ -pthread $^

.PHONY: tests
tests: mwc
	BIN=./$< tests/test.sh

# end
