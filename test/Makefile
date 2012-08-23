check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

program_OBJS := $(wildcard ../lib/*.o)



CFLAGS += -L/opt/local/lib -I/opt/local/include

.PHONY: all clean copy-to-test

all: $(check_NAME)

$(check_NAME): $(check_OBJS)
	$(CC) $(CFLAGS) -o $(check_NAME) $(check_LIBS) $(check_OBJS) $(program_OBJS)

clean :
	$(RM) $(check_NAME) $(check_OBJS) $(check_NAME) *.o
