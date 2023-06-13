/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test3.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/07 16:13:38 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 13:53:02 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For read
#include <string.h>
#include <stdio.h>

#define MAX_LINE 100
#define SERVER_PORT 18001

#define SA struct sockaddr

int main(void)
{
	int listenfd;
	int connfd;
	struct sockaddr_in servaddr;
	char recvline[MAX_LINE + 1];
	std::string		req_recived;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		std::cerr << "Socket error !" << std::endl;
		return (EXIT_FAILURE);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	/* initalisation du server */
	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		std::cerr << "Impossible de bind !" << std::endl;
		perror("bind");
		return (EXIT_FAILURE);
	}
	if (listen(listenfd, 10) < 0)
	{
		std::cerr << "Impossible de listen !" << std::endl;
		perror("listen");
		return (EXIT_FAILURE);
	}
	
	/* maintenant qu'on a configurer ca, on rentre dans la boucle*/
	while (1)
	{
		struct sockaddr_in addr;
		socklen_t addr_len;

		std::cout << "J'attends des connexions sur le port "<< SERVER_PORT << std::endl;
		connfd = accept(listenfd, (SA *) &addr, &addr_len); // les NULL => on regarde pas qui tente de nous parler.
		if (connfd == -1) {
            perror("accept");
            continue;
        }
		/*std::cout << "addr.sin_addr.s_addr : "<< addr.sin_addr.s_addr << std::endl;
		std::cout << "addr.sin_family : "<< addr.sin_family << std::endl;
		std::cout << "addr.sin_port : "<< addr.sin_port << std::endl;
		std::cout << "addr.sin_zero : "<< addr.sin_zero << std::endl;*/
		
		//connfd est le socket que l'on utilise pour interagir avec le client.
		memset(recvline, 0, MAX_LINE+1);
		while (recv(connfd, recvline, MAX_LINE, 0) > 0)
		{
			req_recived += recvline;
			memset(recvline, 0, MAX_LINE+1);
			if (req_recived.size() >= 4 && req_recived.substr(req_recived.size() - 4, 4) == "\r\n\r\n")
			{
				std::cout << "YES END" << std::endl;
				break;
			}
		}
		
		std::cout << req_recived << std::endl;
		req_recived.clear();
		std::string response("HTTP/1.0 200 OK\r\n\r\nHello");
		write (connfd, response.c_str(), response.size() );
		close (connfd);
		
	}
	return (0);
}
