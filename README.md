# pushy [![Build Status](https://travis-ci.org/TomRegan/pushy.png?branch=master)](https://travis-ci.org/TomRegan/pushy)

Pushy is some kind of HTTP server.

### progress

Pushy responds to most requests with a 404 header and a bit of json,
which is nice if you like that sort of thing. Occasionally if you
request the right resource, pushy may gratify you with a lower numbered
response. This is one of pushy's few (seldom) working features.

### one of these days
I started writing pushy to:

* practice some TDD in C
* learn a little bit about HTTP servers, push servers and perhaps
    websockets.
