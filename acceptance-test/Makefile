test_NAME := test_runner
test_SRCS := $(wildcard *.c)
test_OBJS := ${test_SRCS:.c=.o}

CFLAGS += -lcgreen -lm

all: clean $(test_NAME)

$(test_NAME): $(test_OBJS)
	$(CC) $(test_OBJS) $(CFLAGS) -o $(test_NAME)

clean:
	$(RM) $(test_NAME) *.o