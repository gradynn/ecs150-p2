# Target library
lib := libuthread.a
obj := queue.o

CC := GCC
CFLAGS := -Wall -Werror -Wextra

all: $(lib)

$(lib): $(obj)
	ar rcs $@ $(obj)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(lib) $(obj)