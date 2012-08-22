check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

CFLAGS += -I /opt/local/include -L /opt/local/lib

.PHONY: all clean

all: $(check_NAME)

$(check_NAME): $(check_OBJS)
	$(CC) -o $(check_NAME) $(check_LIBS) $(check_OBJS)

clean :
	$(RM) $(check_NAME) $(check_OBJS) $(check_NAME)
