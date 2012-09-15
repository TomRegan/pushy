#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "../include/protocol_handler.h"
#include "../include/buffers.h"
#include "../include/logging.h"

#define PORT 55080
#define BACKLOG 50

#define handle_error(msg) do { perror(msg); fprintf(stderr, msg); exit(EXIT_FAILURE); } while (0)

/**
 * init_server
 *
 * opens a listening socket
 *
 * @return  int  file descriptor of a new socket
 */
int
init_server()
{
	int			sockfd;
	struct sockaddr_in local_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		handle_error("socket");
	}
	bzero(&local_addr, sizeof(struct sockaddr_in));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((bind(sockfd, (struct sockaddr *)&local_addr,
		  sizeof(struct sockaddr_in))) == -1) {
		handle_error("bind");
	}
	if (listen(sockfd, BACKLOG) == -1) {
		handle_error("listen");
	}
	return sockfd;
}

/**
 * accept_request
 *
 * handles socket requests
 *
 * @param  request  listening socket descriptor
 * @param  *peer_addr  sockaddr_in struct
 *
 * @returns  void
 */
void
accept_request(int peerfd, struct sockaddr_in *peer_addr)
{
	char		msg_buffer[HTTP_RESPONSE_LEN + 1];
	char		*status;
	struct request	r;
	int			error = 0;

	printf("\n>>> %s\n\n", inet_ntoa(peer_addr->sin_addr));

	if ((error = read_request(&r, peerfd)) > 0) {
		printf("read %i bytes\n", error);

		if (error > MAX_URI_LEN) {
			status = SURITOOLONG;
		} else {
			status = SNOTFOUND;
		}

		send_response(peerfd, msg_buffer, status, &r);
		printf("\n<<< %s\n\n", inet_ntoa(peer_addr->sin_addr));
		puts(msg_buffer);
	} else {
		printf("error:\t%i\n", error);
	}

	return;
}

/**
 * serve_forever
 *
 * runs in an accept loop responding to connections
 *
 * @param  sockfd  file descriptor of a listening socket
 *
 * @return  int  EXIT_SUCCESS|EXIT_FAILURE
 */
int
serve_forever(int sockfd)
{
	int			peerfd;
	struct sockaddr_in peer_addr;
	socklen_t	sin_size = sizeof(struct sockaddr_in);

	printf("accepting connections on %i\n", PORT);

	while (1) {

		bzero(&peer_addr, sizeof(struct sockaddr_in));
		if ((peerfd = accept(sockfd, (struct sockaddr *)&peer_addr,
				     (socklen_t *) & sin_size)) == -1) {
			handle_error("accept");
		} else {
			/* thread */
			accept_request(peerfd, &peer_addr);
		}
	}

	return EXIT_SUCCESS;
}

int
main(int argc, char *argv[])
{
	int			sockfd;

	printf("Pushy/0.0.1.1 starting\n");

	sockfd = init_server();
	serve_forever(sockfd);

	return EXIT_SUCCESS;
}
