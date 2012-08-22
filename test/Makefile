check_NAME := test_runner
check_SRCS := $(wildcard *.c)
check_OBJS := ${check_SRCS:.c=.o}
check_LIBS := -lcheck

CFLAGS += -I /opt/local/include -L /opt/local/lib -lcheck

.PHONY: all clean

all: $(check_NAME)

$(check_NAME): $(check_OBJS)
	$(CC) $(check_LIBS) $(check_OBJS) -o $(check_NAME)

clean :
	$(RM) $(check_NAME) $(check_OBJS) $(check_NAME)
