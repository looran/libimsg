CFLAGS += -I../ -L../ -Wall -g -O0
LDFLAGS += -L../ -limsg -levent

SOURCES = $(shell echo *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGETS = $(SOURCES:.c=)

all: $(TARGETS)

run: $(TARGETS)
	@count=0 ;\
	errors=0 ;\
	for test in $(TARGETS); do \
		echo =============================================================================== ;\
		echo $$test ;\
		LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../ ./$$test ;\
		if [ $$? -eq 0 ]; then \
			echo OK ;\
		else \
			echo FAILED ;\
			errors=$$(($$errors + 1)) ;\
		fi ;\
		count=$$(($$count + 1)) ;\
	done ;\
	echo =============================================================================== ;\
	echo "$$count tests executed, $$errors errors" ;\
	exit $$errors

run_loop:
	@loopcount=20 ;\
	count=1 ;\
	while [ $$count -lt $$loopcount ]; do \
		echo "###############################################################################" ;\
		make run ;\
		if [ $$? -ne 0 ]; then \
			echo ERROR ;\
			echo "###############################################################################" ;\
			echo "$$count / $$loopcount loops of tests executed, last one did ERROR" ;\
			exit $$? ;\
		fi ;\
		count=$$(($$count + 1)) ;\
	done ;\
	echo "###############################################################################" ;\
	echo "$$count / $$loopcount loops of tests executed, all OK" ;\
	exit 0

clean:
	rm -f $(TARGETS) $(OBJECTS)

.PHONY: run
