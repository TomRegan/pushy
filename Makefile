ifndef V
	QUIET = @
endif

TARGET := build

all: httpd unit-test

httpd:
	+$(QUIET)$(MAKE) -C src

unit-test:
	+$(QUIET)$(MAKE) -C tests/unit

acceptance:
	+$(QUIET)$(MAKE) -C tests/acceptance

check: all
	+$(QUIET)$(MAKE) -C tests/unit check

clean:
	+$(QUIET)$(MAKE) -C src clean
	+$(QUIET)$(MAKE) -C tests/unit clean
	$(QUIET)test ! -d $(TARGET) || rmdir $(TARGET)
	$(QUIET)test ! -d lib/ || rm -r lib/
