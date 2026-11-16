.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment.")
endif

TITLE    := Fighter Jet PS3 Demo v0.3 [1116_0545]
ICON0    := $(CURDIR)/pkg/ICON0.PNG
PIC1     := $(CURDIR)/PIC1.PNG
PIC1     := $(CURDIR)/PIC1.PNG
APPID    := FIGHTER3D
CONTENTID:= UP0001-$(APPID)_00-0000000000000000
APPVER   := 01.03

PKGFILES := $(CURDIR)/pkg
include $(PSL1GHT)/ppu_rules

TARGET   := ps3-fighter_0.3
BUILD    := build
SOURCES  := source
LIBS     := -lfont3d -ltiny3d -laudio -lrsx -lsimdmath -lgcm_sys -lio -lsysutil -lrt -llv2 -lm
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
