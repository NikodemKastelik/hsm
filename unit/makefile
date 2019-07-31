#----------------------------------------------------------#
# @brief  Sample makefile for unit test creation.
#
# Remember that any .c file to test has to be included
# directly to access all static members.
#----------------------------------------------------------#

# @brief Unit test name.
PROJNAME = hsm_test

#----------------------------------------------------------#
# Do not edit anything beneath!
#----------------------------------------------------------#
UNITYDIR = ./unity

SRCDIR   = $(UNITYDIR)
SRCDIR  += .

INCDIR   = $(UNITYDIR)
INCDIR  += ../inc
INCDIR  += ../src

BUILDDIR = build

CC       = gcc

CFLAGS   = -std=c99 -Wall

DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILDDIR)/$*.Td

SRC_C   += $(wildcard $(addsuffix /*.c, $(SRCDIR)))

OBJ      = $(addprefix $(BUILDDIR)/, $(notdir $(SRC_C:.c=.o)))

EXEC     = $(BUILDDIR)/$(PROJNAME)

POSTCC   = @mv -f $(BUILDDIR)/$*.Td $(BUILDDIR)/$*.d && touch $@

$(shell mkdir -p $(BUILDDIR))

vpath %.c $(SRCDIR)

all: TEST

TEST: $(EXEC)
	./$<

$(EXEC): $(OBJ)
	$(CC) $^ -o $@

$(BUILDDIR)/%.o : %.c
$(BUILDDIR)/%.o : %.c $(BUILDDIR)/%.d
	$(CC) $(DEPFLAGS) $(CFLAGS) $(addprefix -I,$(INCDIR)) -c $< -o $@
	$(POSTCC)

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)

$(BUILDDIR)/%.d: ;
.PRECIOUS: $(BUILDDIR)/%.d

include $(wildcard $(patsubst %,$(BUILDDIR)/%.d,$(basename $(SRC_C))))
