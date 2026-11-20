TARGET := XAVRX-PS-VITA-3DS
BUILD := build
SOURCES := source
INCLUDES := include
ARCH := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS := -g -Wall -Wextra -O2 -std=gnu11 -mword-relocations -ffunction-sections $(ARCH) -DARM11 -D_3DS
CFLAGS += $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := $(ARCH)
LDFLAGS := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS := -lctru
LIBDIRS :=
.PHONY: all clean test
test:
	$(CC) -std=c11 -Wall -Wextra -Werror -Iinclude tests/test_vita_elf.c source/vita_elf.c -o /tmp/xavrx_elf_tests
	/tmp/xavrx_elf_tests
ifneq ($(BUILD),$(notdir $(CURDIR)))
all:
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile clean
else
SOURCES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(CURDIR)/$(dir)/*.c)))
CFILES := $(filter %.c,$(SOURCES))
OFILES := $(CFILES:.c=.o)
all: $(TARGET).3dsx
%.o: $(CURDIR)/source/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LIBDIRS) $(LIBS) -o $@
$(TARGET).3dsx: $(TARGET).elf
	3dsxtool $< $@
clean:
	@rm -f $(OFILES) $(TARGET).elf $(TARGET).3dsx $(TARGET).smdh
endif