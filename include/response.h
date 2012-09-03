#ifndef RESPONSE_H
#define RESPONSE_H 1

#define RFC1123_TIME "%a, %d %b %Y %H:%M:%S GMT"
#define RFC1123_TIME_LEN 30

/* error status messages */
#define SNOTFOUND "not-found"
#define SURITOOLONG "uri too long"

/**
 * Builds a response string and sends it to a peer
 * @param peerfd descriptor of the peer
 * @param msg_buf a buffer that will hold the complete message
 * @param msg_content the message content
 * @param r an http request struct
 * @return >=0 on success, -1 on error
 */
int send_response(int, char*, char*, struct request*);

#endif /* RESPONSE_H */
