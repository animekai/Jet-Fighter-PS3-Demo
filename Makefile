.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment.")
endif

TITLE    := PS3 Fighter Stack
APPID    := FIGHTER3D
CONTENTID:= UP0001-$(APPID)_00-0000000000000000

include $(PSL1GHT)/ppu_rules

TARGET   := $(notdir $(CURDIR))
BUILD    := build
SOURCES  := source
LIBS     := -ltiny3d -lrsx -lsimdmath -lgcm_sys -lio -lsysutil -lrt -llv2 -lm
CFLAGS   = -O2 -Wall -mcpu=cell $(MACHDEP) $(INCLUDE)
LDFLAGS  = $(MACHDEP)

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT   := $(CURDIR)/$(TARGET)
export VPATH    := $(CURDIR)/$(SOURCES)
export DEPSDIR  := $(CURDIR)/$(BUILD)
export BUILDDIR := $(CURDIR)/$(BUILD)
CFILES   := $(notdir $(wildcard $(SOURCES)/*.c))
export LD       := $(CC)
export OFILES   := $(CFILES:.c=.o)
export INCLUDE  := $(LIBPSL1GHT_INC) -I$(CURDIR)/$(BUILD) -I$(PORTLIBS)/include
export LIBPATHS := $(LIBPSL1GHT_LIB) -L$(PORTLIBS)/lib

.PHONY: $(BUILD) clean
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
pkg: $(BUILD) $(OUTPUT).pkg

clean:
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).fake.self $(OUTPUT).self EBOOT.BIN
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
