ifndef V
	QUIET = @
endif
check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

TARGET := ../../build/unit-test

program_OBJS := $(wildcard ../../lib/*.o)

CFLAGS += -L/opt/local/lib -I/opt/local/include -I../../include -Qunused-arguments

.PHONY: all clean copy-to-test

all: $(check_NAME)

$(check_NAME): $(check_OBJS)
	$(QUIET)test -d $(TARGET) || mkdir $(TARGET)
	$(QUIET)$(CC) $(CFLAGS) -o $(TARGET)/$(check_NAME) $(check_OBJS) $(program_OBJS) $(check_LIBS)

check: all
	$(QUIET)$(TARGET)/./test_runner

clean :
	$(QUIET)$(RM) $(check_NAME) $(check_OBJS) $(TARGET)/$(check_NAME) *.o
	$(QUIET)test ! -d $(TARGET) || rmdir $(TARGET)
