#ifndef NETWORK_H_
#define NETWORK_H_ 1

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

int screener_listen(const char *hostname, unsigned short port);

#endif
