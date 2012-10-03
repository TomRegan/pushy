TARGET := build

all: httpd unit-test

httpd:
	$(MAKE) -C src

unit-test:
	+$(MAKE) -C tests/unit

at: httpd
	+$(MAKE) -C tests/acceptance

check: all
	+$(MAKE) -C tests/unit check

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C tests/unit clean
	test ! -d $(TARGET) || rmdir $(TARGET)
	test ! -d lib/ || rm -r lib/
