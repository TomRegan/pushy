#ifndef RESPONSE_H
#define RESPONSE_H 1

#define RFC1123_TIME "%a, %d %b %Y %H:%M:%S GMT"
#define RFC1123_TIME_LEN 30
void respond_uritoolong(char * msg_body, struct request *req);
void respond_notfound(char * msg_body, struct request *req);
int send_response(int, char*, struct request*, void (*request_fp)(char *, struct request *) );

#endif /* RESPONSE_H */
