ifndef V
	QUIET := @
endif
program_NAME := httpd
program_SRCS := $(wildcard *.c)
program_OBJS := ${program_SRCS:.c=.o}

LIBDIR := ../lib/
OBJS := protocol_handler.o logging.o cache.o
TARGET := ../build

ifeq ($(mode),release)
	CFLAGS += -DRELEASE
else
	CFLAGS += -g
endif

CFLAGS += -Qunused-arguments

.PHONY: all clean deploy-libs

all: $(program_NAME) deploy-libs

$(program_NAME): $(program_OBJS)
	$(QUIET)test -d $(TARGET) || mkdir $(TARGET)
	$(QUIET)$(CC) $(program_OBJS) -o $(TARGET)/$(program_NAME)

deploy-libs:
	$(QUIET)test -d $(LIBDIR) || mkdir $(LIBDIR)
	$(QUIET)cp $(OBJS) $(LIBDIR)

clean:
	$(QUIET)$(RM) $(program_NAME) $(program_OBJS) \#*\# *~
	$(QUIET)$(RM) $(TARGET)/$(program_NAME)
