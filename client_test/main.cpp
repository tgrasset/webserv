/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/21 11:59:37 by mbocquel          #+#    #+#             */
/*   Updated: 2023/08/10 19:00:17 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

#define SERVER_PORT                "5403"

int main (int argc, char **argv)
{
    if (argc != 2) 
	{
        std::cerr << "Usage: client hostname" << std::endl;
        return(EXIT_FAILURE);
    }

    struct addrinfo hints;
    memset(&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *result;
    int s; 
    if ((s = getaddrinfo (argv [1], SERVER_PORT, &hints, &result)) != 0)
	{
        std::cerr << "getaddrinfo: %s\n" << strerror(errno) << std::endl;
        return(EXIT_FAILURE);
    }

    /* Scan through the list of address structures returned by 
       getaddrinfo. Stop when the the socket and connect calls are successful. */

    int sock_fd;
    socklen_t length;
    struct addrinfo *rptr;

	std::cout << "I am connecting to the server" << std::endl << std::endl;
    for (rptr = result; rptr != NULL; rptr = rptr -> ai_next)
	{
        sock_fd = socket (rptr -> ai_family, rptr -> ai_socktype, rptr -> ai_protocol);
        if (sock_fd == -1)
            continue;
        if (connect (sock_fd, rptr -> ai_addr, rptr -> ai_addrlen) == -1)
		{
            if (close (sock_fd) == -1)
				return (EXIT_FAILURE);
            continue;
        }     
        break;
    }
    if (rptr == NULL)
	{               // Not successful with any address
		std::cerr << "Not able to connect" << std::endl;
        return(EXIT_FAILURE);
    }
    freeaddrinfo (result);

	std::cout << "I am sleeping for a few seconds" << std::endl << std::endl;
	usleep(5 * 1000 * 1000); // Wait for 5 secondes

	std::cout << "I am sending the request" << std::endl;
	std::string request = "GET / HTTP/1.1\r\nHost: localhost:5403\r\nConnection: closed\r\n\r\n";
	send(sock_fd, request.c_str(), request.size(), 0);
	usleep(5 * 1000 * 1000); // Wait for 5 secondes

	/*std::cout << "What is the response ?" << std::endl << std::endl;
	char buffer[4097];
	memset(buffer, 0, 4097);

	std::string res;
	int byte_recv = 0;

	while (recv(sock_fd, buffer, 4096, 0) > 0)
	{
		res += buffer;
		memset(buffer, 0, 4097);
		
	}

	std::cout << "res size "<< res.size() << std::endl;
	
	
	std::cout << "I am waiting a bit and sending another request" << std::endl;
	usleep(2 * 1000 * 1000); // Wait for 5 secondes
	request = "GET / HTTP/1.1\r\nHost: localhost:5403\r\nConnection: closed\r\n\r\n";
	int bsent = send(sock_fd, request.c_str(), request.size(), 0);
	std::cout << bsent << " was sent "<< std::endl;

	std::cout << "What is the response ?" << std::endl << std::endl;
	memset(buffer, 0, 4097);
	res = "";
	byte_recv = 0;

	while (recv(sock_fd, buffer, 4096, 0) > 0)
	{
		res += buffer;
		memset(buffer, 0, 4097);
	}

	std::cout << res << std::endl;
	
	
	std::cout << "I am closing the fd" << std::endl;
	close (sock_fd);

	*/
	return (0);
}
