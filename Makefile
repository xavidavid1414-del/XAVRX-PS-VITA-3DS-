TARGET := XAVRX-PS-VITA-3DS
BUILD := build
ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
CC := arm-none-eabi-gcc
HOST_CC := gcc
ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS := -g -Wall -Wextra -O2 -std=gnu11 -mword-relocations -ffunction-sections $(ARCH) -DARM11 -D_3DS -I$(ROOT)/include
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := $(ARCH)
LDFLAGS := -specs=3dsx.specs -g $(ARCH)
LIBS := -lctru
LIBDIRS :=
.PHONY: all clean test
all:
	@mkdir -p $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(ROOT)/Makefile build-local
test:
	$(HOST_CC) -std=c11 -Wall -Wextra -Werror -I$(ROOT)/include $(ROOT)/tests/test_vita_elf.c $(ROOT)/source/vita_elf.c -o /tmp/xavrx_elf_tests
	/tmp/xavrx_elf_tests
clean:
	@rm -rf $(BUILD)
.PHONY: build-local
build-local:
$(eval SOURCES := $(notdir $(wildcard $(ROOT)/source/*.c)))
$(eval OFILES := $(SOURCES:.c=.o))
build-local: $(TARGET).3dsx
%.o: $(ROOT)/source/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LIBDIRS) $(LIBS) -o $@
$(TARGET).3dsx: $(TARGET).elf
	3dsxtool $< $@