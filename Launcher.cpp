/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/14 13:24:04 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Launcher.hpp"

bool	Launcher::_verbose = true;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Launcher::Launcher(std::string path)
{
	if (Launcher::_verbose)
		std::cout << "Launcher path constructor called" << std::endl;
	this->_path_conf = path;
	return ;
}

Launcher::Launcher(Launcher const & copy)
{
	*this = copy;
	if (Launcher::_verbose)
		std::cout << "Launcher copy constructor called" << std::endl;
}

Launcher::~Launcher(void)
{
	// Closing all socket
	close(_efd);
	for (std::vector<Client>::iterator it = this->_clients.begin(); 
		it != this->_clients.end(); ++it)
		close(it->getCom_socket());
	for (std::vector<Server>::iterator it = this->_servers.begin(); 
		it != this->_servers.end(); ++it)
		close(it->getListenSocket());
	if (Launcher::_verbose)
		std::cout << "Launcher destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
Launcher & Launcher::operator=(Launcher const & launcher)
{
	if (this != &launcher)
	{
		this->_servers = launcher._servers;
		this->_clients = launcher._clients;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

void	Launcher::parse(void)
{
	ConfParser conf_parser(this->_path_conf);
	conf_parser.parse();
	this->_servers = conf_parser.getServers();
}

std::vector<Server>::iterator 	Launcher::getServerWithSameHostPort(std::vector<Server>::iterator it_find)
{
	for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end();
	++it)
	{
		if (it == it_find)
			return (this->_servers.end());
		if (it->getHost() == it_find->getHost() && it->getPort() == it_find->getPort())
			return (it);
	}
	return (this->_servers.end());
}

void	Launcher::launch_servers(void)
{
	struct epoll_event	ev;
	_ep_event.reserve(this->_servers.size() * MAX_EVENTS);
	int nfds = 0;
			
	_efd = epoll_create1 (0);
	if (_efd == -1)
		throw LauncherException("Probleme avec epoll_create !");
		
	/*I add all listen socket to the monitoring of epoll*/
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (this->getServerWithSameHostPort(it) != this->_servers.end())
			it->setListenSocket(this->getServerWithSameHostPort(it)->getListenSocket());
		else
		{
			int listensocket = socket(AF_INET, SOCK_STREAM, 0);
			if (listensocket < 0)
				throw LauncherException("Problem with socket !");
			//setsockopt(listensocket, int level, int option_name, const void *option_value, socklen_t option_len);
			it->setListenSocket(listensocket);
			it->setServaddr();
			it->bind_server();
			if (listen(it->getListenSocket(), MAX_WAIT) < 0)
				throw LauncherException("Impossible de listen !");
		}
		ev.events = EPOLLIN;
		ev.data.fd = it->getListenSocket();
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, it->getListenSocket(), &ev) == -1)
			throw LauncherException("\e[31mProblem with epoll_ctl ! 1\e[0m");
	}
		
	/* Global loop for the Webserv */
	while (1)
	{
		/*Block until there is something to do ...*/
		this->print_ep_event();
		if ((nfds = epoll_wait(_efd, static_cast< struct epoll_event * >(_ep_event.data()), this->_servers.size() * MAX_EVENTS, -1)) == -1)
			throw LauncherException("Problem with epoll_wait !");
		// Some sockets are ready. Examine readfds
        for (int i = 0; i < nfds; i++)
		{
			if ((_ep_event[i].events & EPOLLIN) == EPOLLIN) // File descriptor is available for read.
			{
				if (check_if_listen_socket(_ep_event[i].data.fd )) //new client wants to connect
					this->process_new_client(i);
				else
					this->process_reading_existing_client(i);				
			}
			else if ((_ep_event[i].events & EPOLLOUT) == EPOLLOUT) // File descriptor is available for write.
			{
				
			}
			else if ((_ep_event[i].events & EPOLLRDHUP) == EPOLLRDHUP) // Stream socket peer closed connection.
			{
				
			}
		}
	}
}

void	Launcher::process_new_client(int i)
{
	socklen_t 			addrlen;
	struct epoll_event	ev;
	struct sockaddr_in  client_addr;
	char				str_ip_client[INET_ADDRSTRLEN];
	
	addrlen = sizeof (struct sockaddr_in);
	int fd_new;
	fd_new = accept(_ep_event[i].data.fd, (struct sockaddr *) &client_addr, &addrlen);
	if (fd_new == -1)
		throw LauncherException("Problem with accept !");
	
	// add fd_new to epoll
	ev.events = EPOLLIN;
	ev.data.fd = fd_new;
	if (epoll_ctl(_efd, EPOLL_CTL_ADD, fd_new, &ev) == -1)
		throw LauncherException("Problem with epoll_ctl ! 2");
	
	// I add a new client to my vector of client
	Client	new_client(client_addr, fd_new);
	this->add_server_of_client(_ep_event[i].data.fd, &new_client);
	_clients.push_back(new_client);
	
	// just for log information
	inet_ntop (AF_INET, &(client_addr.sin_addr), str_ip_client, sizeof (str_ip_client));
	std::cout << "	I just accepted a new connexion from " << str_ip_client << std::endl;
}

bool	Launcher::check_if_listen_socket(int socket)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (socket == it->getListenSocket())
			return (true);
	}
	return (false);
}

void 	Launcher::add_server_of_client(int listen_sock, Client *client)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		if (listen_sock == it->getListenSocket())
			client->AddServerPtr(&(*it));
	}
}

void	Launcher::process_reading_existing_client(int i)
{
	int byte_recv = 0;
	std::vector< Client >::iterator it = this->_clients.begin();
	char recvline[BUFFER_SIZE + 1];
	memset(recvline, 0, BUFFER_SIZE + 1);

	while (it != this->_clients.end())
	{
		if (it->getCom_socket() == _ep_event[i].data.fd)
			break ;
		++it;
	}
	if (it == this->_clients.end())
		throw LauncherException("Error: Client not found !");
	byte_recv = recv(_ep_event[i].data.fd, recvline, BUFFER_SIZE, 0);
	if (byte_recv > 0)
	{
		it->SetStatus(SENDING_REQ);
		it->add_to_req_recived(recvline);
	}
	// if the client has finish sending the request, I ; 
	if (it->getStatus() == REQ_SENT)
	{
		if (epoll_ctl(_efd, EPOLL_CTL_DEL, _ep_event[i].data.fd, NULL) == -1)
	       throw LauncherException("Error: epoll_ctl DEL!");
	}
	
}

void	Launcher::print_ep_event(void)
{
	std::cout << "Waiting for an event :" << std::endl;
	std::cout << "	Server listen socket : ";
	for (std::vector<Server>::iterator it = this->_servers.begin();
		it != this->_servers.end(); ++it)
	{
		std::cout << it->getListenSocket() << " ";
	}
	std::cout << std::endl << "	Client sending request : ";
	for (std::vector<Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
	{
		if (it->getStatus() == WANT_TO_SEND_REQ || it->getStatus() == SENDING_REQ)
			std::cout << it->getCom_socket() << " ";
	}
	std::cout << std::endl << "	Client waiting for response : ";
	for (std::vector<Client>::iterator it = this->_clients.begin();
		it != this->_clients.end(); ++it)
	{
		if (it->getStatus() == REQ_SENT)
			std::cout << it->getCom_socket() << " ";
	}
	std::cout << std::endl << std::endl;
}

//std::vector<struct epoll_event>	_ep_event;

/*if ((_ep_event[i].events & EPOLLIN) == EPOLLIN) // File descriptor is available for read.
{
	if (check_if_listen_socket(i)) //new client wants to connect
		this->process_new_client(i);
	else
		this->process_reading_existing_client(i);				
}
else if ((_ep_event[i].events & EPOLLOUT) == EPOLLOUT) // File descriptor is available for write.
{
	
}
else if ((_ep_event[i].events & EPOLLRDHUP) == EPOLLRDHUP) // Stream socket peer closed connection.
{
	
}*/