PROJNAME = hsm

SHELL    = /bin/bash

SRCDIR   = ./src
BUILDDIR = ./build
INCDIR   = ./inc

CC       = gcc
OBJCOPY  = objcopy

CFLAGS   = -std=gnu99 -Wall -Og -ggdb
CFLAGS  += $(addprefix -I,$(INCDIR))
CFLAGS  += -ffunction-sections -fdata-sections

LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,-Map=$(BUILDDIR)/$(PROJNAME).map

DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILDDIR)/$*.Td

SRC_C    = $(wildcard $(addsuffix /*.c, $(SRCDIR)))
SRC_S    = $(wildcard $(addsuffix /*.S, $(SRCDIR)))

OBJ      = $(addprefix $(BUILDDIR)/, $(notdir $(SRC_S:.S=.o)))
OBJ     += $(addprefix $(BUILDDIR)/, $(notdir $(SRC_C:.c=.o)))

ELF      = $(addsuffix .elf, $(BUILDDIR)/$(PROJNAME))

POSTCC   = @mv -f $(BUILDDIR)/$*.Td $(BUILDDIR)/$*.d && touch $@

$(shell mkdir -p $(BUILDDIR))

vpath %.c $(SRCDIR)
vpath %.S $(SRCDIR)

all: $(ELF)

$(ELF): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o : %.c
$(BUILDDIR)/%.o : %.c $(BUILDDIR)/%.d
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@
	$(POSTCC)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

.PHONY: print
print:
	@echo $(SRC_C)
	@echo $(SRC_S)

$(BUILDDIR)/%.d: ;
.PRECIOUS: $(BUILDDIR)/%.d

include $(wildcard $(patsubst %,$(BUILDDIR)/*.d,$(basename $(SRC_C))))
