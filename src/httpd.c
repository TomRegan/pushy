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
#include <signal.h>
#include "../include/protocol_handler.h"
#include "../include/buffers.h"
#include "../include/logging.h"
#include "../include/cache.h"

#define PORT 55080
#define BACKLOG 50

#define handle_error(msg) do { perror(msg); fprintf(stderr, msg); exit(EXIT_FAILURE); } while (0)

void
_shutdown_hook(int ignored)
{
    puts("Shutting down pushy...");
    exit(0);
}

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
	int		sockfd;
	struct sockaddr_in local_addr;

	cache_init();

	cache_add("SYSTEM", "{\"request\":\"/SYSTEM\",\"version\":\"0.0.1.1\"}");

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
	char           *status;
	struct request	req;
	int		nbytes = 0;

	while ((nbytes = read_request(peerfd, &req)) > 0) {
		log_conn(FINE, peer_addr, ">>> %s\n", req.uri);
		send_response(peerfd, msg_buffer, SNOTFOUND, &req);
		if (nbytes) {
			log_ln(FINER, "read %i bytes\n", nbytes);
			log_conn(FINE, peer_addr, "<<< %s\n", msg_buffer);
		}
	}

	if (nbytes == ECONNRST) {
		log_conn(FINE, peer_addr, "connection reset by client\n");
	} else if (nbytes == EREADHEAD) {
		log_ln(ERROR, "reading request header: 0x%02x\n", (int) abs(nbytes));
	} else if (nbytes == ESOCKERR) {
		log_ln(ERROR, "reading from socket: 0x%02x\n", (int) abs(nbytes));
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
	int		peerfd, pid;
	socklen_t	sin_size = sizeof(struct sockaddr_in);
	struct sockaddr_in peer_addr;

	printf("accepting connections on %i\n", PORT);

	while (1) {

		bzero(&peer_addr, sizeof(struct sockaddr_in));
		if ((peerfd = accept(sockfd, (struct sockaddr *)&peer_addr,
				     (socklen_t *) & sin_size)) == -1) {
			/*
			 * TODO:
			 * Introducing semaphores has created an issue where
			 * EINTR is raised somewhere, and causes accept
			 * to fail. This needs investigating.
			 */
			if (errno == EINTR) {
				continue;
			}
			handle_error("accept");
		} else {
			if ((pid = fork()) == 0) {
				close(sockfd);
				log_conn(FINE, &peer_addr, "connected\n");
				accept_request(peerfd, &peer_addr);
				log_conn(FINE, &peer_addr, "done serving request\n");
				exit(0);
			}
			log_ln(DEBUG, "forked process %i\n", pid);
			close(peerfd);
		}
	}

	return EXIT_SUCCESS;
}

int
main(int argc, char *argv[])
{
	int		sockfd;

	printf("Pushy/0.0.1.1 starting\n");

    signal(SIGINT, _shutdown_hook);

	sockfd = init_server();
	serve_forever(sockfd);

	return EXIT_SUCCESS;
}
