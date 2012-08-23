#include <arpa/inet.h>

#ifndef REQUEST_H
#define REQUEST_H 1

#define MAX_URI_LEN 1024

#define MOPTIONS   1
#define MGET       2
#define MHEAD      4
#define MPOST      8
#define MPUT      16
#define MDELETE   32
#define MTRACE    64
#define MUNKNOWN 128

struct
http_version
{
  char major;
  char minor;
};

struct
request
{
  unsigned char method;
  char *uri;
};

int read_request(struct request*, int);
unsigned char _get_request_method(char *);
char * _get_request_uri(char *);

#endif /* REQUEST_H */
