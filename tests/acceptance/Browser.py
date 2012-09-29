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
# @file  Browser.py
# @author  Tom Regan <code.tom.regan@gmail.com>
#

class Chrome(object):
        request = 'GET /chrome-test HTTP/1.1\r\n\
Host: localhost:55080\r\n\
Connection: keep-alive\r\n\
Cache-Control: max-age=0\r\n\
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_2) AppleWebKit/537.4 (KHTML, like Gecko) Chrome/22.0.1229.79 Safari/537.4\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: en-US,en;q=0.8\r\n\
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n\r\n'

        expectBody = '{"request":"/chrome-test","response":"not-found"}'
