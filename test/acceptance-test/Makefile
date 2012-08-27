check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

TARGET := ../../build/acceptance-test

CFLAGS += -L/opt/local/lib -I/opt/local/include -I../../include

.PHONY: all clean

all: $(check_NAME)

$(check_NAME): $(check_OBJS)
	test -d $(TARGET) || mkdir $(TARGET)
	$(CC) -o $(TARGET)/$(check_NAME) $(CFLAGS) $(check_OBJS) $(check_LIBS)

clean:
	$(RM) $(TARGET)/$(check_NAME) *.o
	test ! -d $(TARGET) || rmdir $(TARGET)
