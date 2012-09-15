program_NAME := httpd
program_SRCS := $(wildcard *.c)
program_OBJS := ${program_SRCS:.c=.o}

LIBDIR := ../lib/
OBJS := protocol_handler.o logging.o
TARGET := ../build

ifeq ($(mode),release)
	CFLAGS += -DRELEASE
else
	CFLAGS += -ggdb
endif

.PHONY: all clean deploy-libs

all: $(program_NAME) deploy-libs

$(program_NAME): $(program_OBJS)
	test -d $(TARGET) || mkdir $(TARGET)
	$(CC) $(program_OBJS) -o $(TARGET)/$(program_NAME)

deploy-libs:
	test -d $(LIBDIR) || mkdir $(LIBDIR)
	cp $(OBJS) $(LIBDIR)

clean:
	$(RM) $(program_NAME) $(program_OBJS) \#*\# *~
	$(RM) $(TARGET)/$(program_NAME)
