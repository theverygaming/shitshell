ABI = linux
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
CFLAGS = -fno-pie
LDFLAGS = -nostdlib -no-pie

SHITSHELL_CFLAGS += -I shitcstd/include -I shitcstd/abi/$(ABI)/include -nostdinc -nostdlib -Wall -fno-stack-protector -ffreestanding $(CFLAGS)
SHITSHELL_LDFLAGS += $(LDFLAGS) --undefined _start

all: shitshell

shitcstd/build/libshitcstd.a:
	@mkdir -p shitcstd/build
	cd shitcstd/build && cmake .. -DCMAKE_C_COMPILER="$(CC)" -DABI=$(ABI) && make

shitshell: shitcstd/build/libshitcstd.a main.o
	@$(LD) $(SHITSHELL_LDFLAGS) $^ shitcstd/build/libshitcstd.a $(shell $(CC) -print-libgcc-file-name) -o $@

%.o: %.c
	@echo "  CC shitshell/$<"
	@$(CC) $(SHITSHELL_CFLAGS) -c $< -o $@

clean:
	@rm -f shitshell *.o
	@rm -rf shitcstd/build/
