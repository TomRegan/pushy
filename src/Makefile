program_NAME := httpd
program_SRCS := $(wildcard *.c)
program_OBJS := ${program_SRCS:.c=.o}

LIBDIR := ../lib
TARGET := ../build
UNAME := $(shell uname)
BUILDS := $(shell git rev-list HEAD --count)

ifeq ($(UNAME),Linux)
	CFLAGS += -pthread
endif

ifeq ($(mode),release)
	CFLAGS += -DRELEASE
else
	CFLAGS += -g
endif

.PHONY: all clean deploy-libs

all: $(program_NAME) deploy-libs

$(program_NAME): ../include/version.h $(program_OBJS)
	test -d $(TARGET) || mkdir $(TARGET)
	$(CC) $(CFLAGS) $(program_OBJS) -o $(TARGET)/$(program_NAME)

deploy-libs:
	test -d $(LIBDIR) || mkdir $(LIBDIR)
	cp $(program_OBJS) $(LIBDIR)
	$(RM) $(LIBDIR)/httpd.o

../include/version.h:
	printf "#define VERSION \"%d.%d.%d (%s)\"\n\n" 0 0 $(BUILDS) $(UNAME) > ../include/version.h

clean:
	$(RM) $(program_NAME) $(program_OBJS) \#*\# *~
	$(RM) $(TARGET)/$(program_NAME)
	$(RM) ../include/version.h
