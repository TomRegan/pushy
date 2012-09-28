#! /usr/bin/python

import unittest, subprocess, time, urllib2

class Test(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        subprocess.Popen("../../build/httpd");
        pass


if __name__ == '__main__':
    response = 'TEST FAILED'
    try:
        print
        p = subprocess.Popen("build/httpd")
        time.sleep(1)
        try:
            response = urllib2.urlopen("http://localhost:55080/python-test")
        except urllib2.URLError, e:
            if not hasattr(e, "code"):
                raise
            response = e
    finally:
        p.kill()
        print response
