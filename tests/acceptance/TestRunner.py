#! /usr/bin/python
#
# Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy
# of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#
#

#
# @file  TestRunner.py
# @author  Tom Regan <code.tom.regan@gmail.com>
#

import unittest, time
from Server import Server
from HttpConnection import HttpConnection
from Browser import Chrome, Firefox

class Tests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        Server.attachLogger('at.log')
        Server.start()

    @classmethod
    def tearDownClass(cls):
        Server.kill()

    def testResponseNotFound(self):

        expectBody = '{"request":"/","response":"not-found"}'
        conn = HttpConnection()
        response = conn.send("GET / HTTP/1.1")
        conn.close()

        self.assertTrue(expectBody in response)

    def testCharacterEncodingSpecified(self):

        expectField = 'Content-Type: application/json; charset=utf-8'

        conn = HttpConnection()
        response = conn.send("GET / HTTP/1.1")
        conn.close()

        self.assertTrue(expectField.lower() in response.lower())

    def testChromeRequest(self):

        for i in range(300):
            now = time.time()
            conn = HttpConnection()
            response = conn.send(Chrome.request)
            conn.close()
            #
            # Each request should takes less than 5 thousandths of
            # of a second. This isn't a functional requirement, it's
            # just to flag up code changes that impact performance.
            #
            self.assertLess((time.time() - now), 0.005)
            self.assertTrue(Chrome.expectBody in response)

    def testFirefoxRequest(self):

        for i in range(300):
            now = time.time()
            conn = HttpConnection()
            response = conn.send(Firefox.request)
            conn.close()
            self.assertLess((time.time() - now), 0.005)
            self.assertTrue(Firefox.expectBody in response)

    def testRequestForSystem(self):

        expectBody = '{"request":"/SYSTEM","response":{"version":"0.0.1.1"}}'

        conn = HttpConnection()
        response = conn.send("GET /SYSTEM HTTP/1.1")
        conn.close()

        self.assertTrue(expectBody in response)


if __name__ == '__main__':
    unittest.main()
