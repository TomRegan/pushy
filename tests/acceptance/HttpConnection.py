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
# @file  HttpConnection.py
# @author  Tom Regan <code.tom.regan@gmail.com>
#

import socket

class HttpConnection(object):

    socket = None
    def __init__(self, host='localhost', port=55080):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((host, port))

    def send(self, message):
        self.socket.sendall(message)
        return self.socket.recv(1024)

    def close(self):
        self.socket.close()
