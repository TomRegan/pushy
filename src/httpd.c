/**
 * Copyright 2012 Tom Regan <code.tom.regan@gmail.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 * 
 */

/**
 * @file  httpd.c
 * @author  Tom Regan <code.tom.regan@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
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
	write_log(DEBUG, "init_server()\n");
	int		sockfd;
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
	write_log(DEBUG, "accept_request()\n");
	static int	req_no;
	char		msg_buffer[HTTP_RESPONSE_LEN + 1];
	char           *status;
	struct request	r;
	int		error = 0;

	write_log(FINE, "request #%i\n", ++req_no);
	write_log(FINE, ">>> %s:%i\n", inet_ntoa(peer_addr->sin_addr), ntohs(peer_addr->sin_port));

	if ((error = read_request(&r, peerfd)) > 0) {
		write_log(FINE, "read %i bytes from %s:%i\n", error, inet_ntoa(peer_addr->sin_addr), ntohs(peer_addr->sin_port)); 

		if (error > MAX_URI_LEN) {
			status = SURITOOLONG;
			write_log(DEBUG, "request too long\n");
		} else {
			status = SNOTFOUND;
			write_log(DEBUG, "request not found\n");
		}
		send_response(peerfd, msg_buffer, status, &r);
		write_log(FINE, "<<< %s:%i\n%s\n", inet_ntoa(peer_addr->sin_addr), ntohs(peer_addr->sin_port), msg_buffer);
	} else {
		write_log(ERROR, "read_request: 0x%02x\n", (int) abs(error));
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
	write_log(DEBUG, "serve_forever()\n");
	int		peerfd, pid;
	struct sockaddr_in peer_addr;
	socklen_t	sin_size = sizeof(struct sockaddr_in);

	write_log(INFO, "accepting connections on %i\n", PORT);

	while (1) {

		bzero(&peer_addr, sizeof(struct sockaddr_in));
		if ((peerfd = accept(sockfd, (struct sockaddr *)&peer_addr,
				     (socklen_t *) & sin_size)) == -1) {
			if (errno == EINTR) {
				continue;
			}
			handle_error("accept");
		} else {
			if ((pid = fork()) == 0) {
				write_log(FINE, "connection from %s:%i\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
				accept_request(peerfd, &peer_addr);
				exit(0);
			}
			write_log(DEBUG, "forked process %i\n", pid);
		}
	}
	write_log(FINEST, "closing connection %s:%i\n", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
	close(sockfd);

	return EXIT_SUCCESS;
}

int
main(int argc, char *argv[])
{
	int		sockfd;

	write_log(INFO, "Pushy/0.0.1.1 starting\n");

	sockfd = init_server();
	serve_forever(sockfd);

	return EXIT_SUCCESS;
}
