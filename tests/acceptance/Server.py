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
# @file  Server.py
# @author  Tom Regan <code.tom.regan@gmail.com>
#

import subprocess, time

class Server(object):

    attempts = 0

    @staticmethod
    def start():
        if Server.attempts > 2:
            exit()
        Server.p = subprocess.Popen("../../build/httpd", stdout=subprocess.PIPE)
        time.sleep(1)
        Server.p.poll()
        if not Server.p.returncode == None:
            print 'Server is already running. Waiting'
            time.sleep(10)
            Server.attempts += 1
            Server.start()

    @staticmethod
    def kill():
        Server.p.kill()

