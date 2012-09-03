TARGET := build

all: httpd acceptance-test unit-test

httpd:
	+$(MAKE) -C src

unit-test:
	+$(MAKE) -C tests/unit

acceptance-test:
	+$(MAKE) -C tests/acceptance

check: all
	+$(MAKE) -C tests/unit check

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C tests/unit clean
	+$(MAKE) -C tests/acceptance clean
	test ! -d $(TARGET) || rmdir $(TARGET)
