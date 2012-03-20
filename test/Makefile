test_NAME := test_runner
test_SRCS := $(wildcard *.c)
test_OBJS := ${test_SRCS:.c=.o}

program_SRCS := $(wildcard ../src/*.c)
program_OBJS := ${program_SRCS:.c=.o}

CFLAGS += -lcgreen -lm

.PHONY: clean dependencies all copy_to_test clean_objs

all: $(program_OBJS) copy_to_test $(test_NAME) clean_objs

$(test_NAME): $(test_OBJS)
	$(CC) $(test_OBJS) request.o $(CFLAGS) -o $(test_NAME)

copy_to_test:
	@mv $(program_OBJS) .
	@$(RM) $(program_OBJS)

clean_objs:
	@$(RM) *.o

clean: clean_objs
	@$(RM) $(test_NAME) *~ \#*\#
