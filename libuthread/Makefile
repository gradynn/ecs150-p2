# Target library
lib := libuthread.a
obj := queue.o uthread.o context.o

CC := gcc
CFLAGS := -Wall -Werror -Wextra

all: $(lib)

$(lib): $(obj)
	ar rcs $@ $(obj)
	
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(lib) $(obj)