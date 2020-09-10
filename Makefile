CC = gcc
LINKER = gcc
CFLAGS = -Wno-deprecated $(shell pkg-config --cflags sdl) -O3
LFLAGS = $(shell pkg-config --libs sdl -ldl)
TARGET = demo3d
SRCDIR = src
OBJDIR = obj

SRC := $(wildcard $(SRCDIR)/*.c)
INC := $(wildcard $(SRCDIR)/*.h)
OBJ := $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

all: $(OBJDIR) $(TARGET)
$(TARGET): $(OBJ)
	$(LINKER) $(OBJ) $(LFLAGS) -o $@
$(OBJ): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR):
	mkdir -p $@
.PHONY: clean
clean:
	rm -rvf $(OBJDIR)
