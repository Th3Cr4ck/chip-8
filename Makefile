CC = gcc
CFLAGS = -Wall -Wextra -Isrc -Iinclude -MMD -MP
LDFLAGS =
SRCDIR = source
BUILDDIR = bin
TARGET = $(BUILDDIR)/chip-8

# Find all source files in src/
SRCS = $(wildcard $(SRCDIR)/*.c)
# Convert source/main.c to build/main.o
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

.PHONY: all clean dirs

all: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | dirs
	$(CC) $(CFLAGS) -c $< -o $@

dirs:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) $(TARGET)

-include $(DEPS)   
