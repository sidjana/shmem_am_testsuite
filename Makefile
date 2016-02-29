
.PHONY: clean run all 

OSHCC = oshcc
SOURCES  = $(wildcard *.c)
EXECS  = $(SOURCES:.c=.x)
CFLAGS = -I ./support/
OSHLAUNCHER = srun 


compile: $(EXECS)  

.SUFFIXES: .x
%.x:	%.c ./support/rtc*
	$(OSHCC) $(CFLAGS) -o $@ $^

run all default: $(EXECS)
	$(OSHLAUNCHER) -N 2 -n 2 bw_1sided.x
	$(OSHLAUNCHER) -N 6 -n 2 ring_multiple-msg_1sided.x
	$(OSHLAUNCHER) -N 6 -n 2 ring_single-msg_1sided.x

clean:
	rm *.x
