# Target library
lib := libuthread.a
obj := queue.o uthread.o context.o sem.o preempt.o

CC := gcc
CFLAGS := -Wall -Werror -Wextra

## Debug flag
ifneq ($(D),1)
CFLAGS    += -O2
else
CFLAGS    += -g
endif

all: $(lib)

$(lib): $(obj)
	ar rcs $@ $(obj)
	
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(lib) $(obj)