
SHMEM_CC = oshcc

SHMEM_CFLAGS = -O2

C_TESTS = $(wildcard *.c)

C_EXECS = $(C_TESTS:.c=.x)

.PHONY: clean all default

all: $(C_EXECS)


.SUFFIXES: .x

%.x: %.c ../support/rtc.c
	$(SHMEM_CC) $(SHMEM_CFLAGS) $^ -o $@


clean:
	rm -rf *.x
