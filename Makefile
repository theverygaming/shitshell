CC = gcc
CFLAGS = -I shitcstd/include -nostdinc -nostdlib -Wall -m32 -march=i386 -fno-pie -fno-stack-protector -ffreestanding -g

all: shitshell

shitcstd/build/libshitcstd.a:
	@mkdir -p shitcstd/build
	@cd shitcstd/build && cmake .. && make

shitshell: shitcstd/build/libshitcstd.a main.o
	@ld -m elf_i386 -static -nostdlib $^ shitcstd/build/libshitcstd.a -o $@

%.o: %.c
	@echo "-> compiling shitshell/$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f shitshell *.o
	@rm -rf shitcstd/build/
