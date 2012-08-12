program_NAME := httpd
program_SRCS := $(wildcard *.c)
program_OBJS := ${program_SRCS:.c=.o}
program_LIBS := 

CFLAGS += -Wall

ifeq ($(mode),release)
	CFLAGS += -DRELEASE
else
	CFLAGS += -ggdb
endif

.PHONY: all clean distclean test

all: $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(CC) $(program_OBJS) -o $(program_NAME)

clean:
	@$(RM) $(program_NAME) $(program_OBJS) \#*\# *~
