CC = gcc
CFLAGS = -I shitcstd/include -nostdlib -Wall -m32 -march=i386 -fno-pie -fno-stack-protector -ffreestanding

all: shitcstd/build/libshitcstd.a shitshell

shitcstd/build/libshitcstd.a:
	@mkdir -p shitcstd/build
	@cd shitcstd/build && cmake .. && make

shitshell: main.o
	@ld -m elf_i386 -static -nostdlib $^ shitcstd/build/libshitcstd.a -o $@

%.o: %.c
	@echo "-> compiling shitshell/$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f shitshell *.oasm *.oc *.o
	@rm -rf shitcstd/build/
