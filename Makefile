TARGET := build

all: httpd acceptance-test unit-test

httpd:
	+$(MAKE) -C src

unit-test:
	+$(MAKE) -C test/unit-test

acceptance-test:
	+$(MAKE) -C test/acceptance-test

check:
	+$(MAKE) -C test/unit-test check

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C test/unit-test clean
	+$(MAKE) -C test/acceptance-test clean
	test ! -d $(TARGET) || rmdir $(TARGET)
