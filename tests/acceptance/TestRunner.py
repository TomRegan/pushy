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

import unittest
from Server import Server
from HttpConnection import HttpConnection

class Tests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
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


if __name__ == '__main__':
    unittest.main()
