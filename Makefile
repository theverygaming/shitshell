CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
CFLAGS = -I shitcstd/include -nostdinc -nostdlib -Wall -fno-pie -fno-stack-protector -ffreestanding -g
LDFLAGS = -nostdlib -no-pie

all: shitshell

shitcstd/build/libshitcstd.a:
	@mkdir -p shitcstd/build
	cd shitcstd/build && cmake .. -DCMAKE_C_COMPILER="$(CC)" && make

shitshell: shitcstd/build/libshitcstd.a main.o
	@$(LD) $(LDFLAGS) $^ shitcstd/build/libshitcstd.a $(shell $(CC) -print-libgcc-file-name) -o $@

%.o: %.c
	@echo "  CC shitshell/$<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f shitshell *.o
	@rm -rf shitcstd/build/
