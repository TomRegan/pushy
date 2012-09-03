TARGET := build

all: httpd acceptance-test unit-test

httpd:
	+$(MAKE) -C src

unit-test:
	+$(MAKE) -C test/unit

acceptance-test:
	+$(MAKE) -C test/acceptance

check:
	+$(MAKE) -C test/unit check

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C test/unit clean
	+$(MAKE) -C test/acceptance clean
	test ! -d $(TARGET) || rmdir $(TARGET)
