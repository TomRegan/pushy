check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

program_SRCS := $(wildcard ../src/*.c)
program_OBJS := ${program_SRCS:.c=.o}

CFLAGS += -L/opt/local/lib -I/opt/local/include

.PHONY: all clean copy-to-test

all: $(program_OBJS) copy-to-test $(check_NAME)

$(check_NAME): $(check_OBJS)
	$(CC) $(CFLAGS) -o $(check_NAME) $(check_LIBS) $(check_OBJS) request.o

copy-to-test:
	mv $(program_OBJS) .

clean :
	$(RM) $(check_NAME) $(check_OBJS) $(check_NAME) *.o
