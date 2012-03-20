program_NAME := httpd
program_SRCS := $(wildcard *.c)
program_OBJS := ${program_SRCS:.c=.o}
program_LIBS := 

CFLAGS += -Wall -ggdb

ifdef RELEASE
CFLAGE += -DRELEASE
endif


.PHONY: all clean distclean test

all: $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(CC) $(program_OBJS) -o $(program_NAME)
	@$(RM) $(program_OBJS)

clean:
	@$(RM) $(program_NAME) $(program_OBJS) \#*\# *~