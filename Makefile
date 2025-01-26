CFLAGS += -Wall -Wextra
CFLAGS += -I/home/xjl/gateway
LDFLAGS += -lpaho-mqtt3c

SRCS += $(shell find app -type f -name "*.c")
SRCS += $(shell find daemon -type f -name "*.c")
SRCS += $(shell find ota -type f -name "*.c")
SRCS += $(shell find thirdparty -type f -name "*.c")

OBJS = $(SRCS:.c=.o)

TARGET = gateway


.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS) main.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS) main.o $(TARGET)

test_buffer: $(OBJS) test/test_buffer.o
	-@$(CC) $(CFLAGS) -o $@ $^
	-@./$@
	-@$(RM) $@ $^

test_mqtt: $(OBJS) test/test_mqtt.o
	-@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	-@./$@
	-@$(RM) $@ $^

test_message: $(OBJS) test/test_message.o
	-@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	-@./$@
	-@$(RM) $@ $^