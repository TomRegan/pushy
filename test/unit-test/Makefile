check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

TARGET := ../../build/unit-test

program_OBJS := $(wildcard ../../lib/*.o)

CFLAGS += -L/opt/local/lib -I/opt/local/include -I../../include

.PHONY: all clean copy-to-test

all: $(check_NAME)

$(check_NAME): $(check_OBJS)
	test -d $(TARGET) || mkdir $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET)/$(check_NAME) $(check_OBJS) $(program_OBJS) $(check_LIBS)

check: all
	$(TARGET)/./test_runner

clean :
	$(RM) $(check_NAME) $(check_OBJS) $(TARGET)/$(check_NAME) *.o
	test ! -d $(TARGET) || rmdir $(TARGET)
