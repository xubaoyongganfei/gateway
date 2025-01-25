CFLAGS += -Wall -Wextra
CFLAGS += -I/home/xjl/gateway
TOOLCHAIN_DIR := /home/xjl/atguigupi

SRCS += $(shell find app -type f -name "*.c")
SRCS += $(shell find daemon -type f -name "*.c")
SRCS += $(shell find ota -type f -name "*.c")
SRCS += $(shell find thirdparty -type f -name "*.c")


OBJS = $(SRCS:.c=.o)
TARGET = gateway

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJS) main.o $(TARGET)

test_buffer: $(OBJS) test/test_buffer.o
	-@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	-@./$@
	-@$(RM) $@ $^




