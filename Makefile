TARGET := build

all: httpd unit-test acceptance-test

httpd:
	+$(MAKE) -C src

unit-test:
	+$(MAKE) -C test/unit-test test

acceptance-test:
	+$(MAKE) -C test/acceptance-test

check:
	+$(MAKE) -C test/unit-test clean
	+$(MAKE) -C test/unit-test test

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C test/unit-test clean
	+$(MAKE) -C test/acceptance-test clean
	test ! -d $(TARGET) || rmdir $(TARGET)
