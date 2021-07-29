CFLAGS += $(EXTRAFLAGS)

LDFLAGS += -T$(LINKER_SCRIPT_RAW)

include nordic/Makefile
