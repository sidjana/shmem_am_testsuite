
SHMEM_CC: oshcc

SHMEM_CFLAGS: -O2

C_TESTS: $(wildcard *.c)

C_EXECS: $(CTESTS:.c=.x)

.PHONY: clean all

.SUFFIXES: .x


all: $(C_EXECS)


%.x: %.c
	$(SHMEM_CC) $(SHMEM_CFLAGS) $^ -o $@


clean:
	rm -rf *.x
