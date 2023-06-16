/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 12:22:34 by mbocquel         ###   ########.fr       */
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
			const int enable = 1;
			if (setsockopt(listensocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
				throw LauncherException("Problem with setsockopt !");
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
		this->print_situation();
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
				this->process_writing_to_client(i);
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
	std::cout << getTimestamp() << "\e[33m	I just accepted a new connexion from " << str_ip_client << "\e[0m" << std::endl;
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
	// if the client has finish sending the request, I remove him from the listen part to add to the send part ; 
	if (it->getStatus() == WAITING_FOR_RES)
	{
		struct epoll_event	ev;
		int fd = _ep_event[i].data.fd;
		ev.events = EPOLLOUT;
		ev.data.fd = fd;
		if (epoll_ctl(_efd, EPOLL_CTL_DEL, _ep_event[i].data.fd, NULL) == -1)
			throw LauncherException("Error: epoll_ctl DEL!");
		if (epoll_ctl(_efd, EPOLL_CTL_ADD, fd, &ev) == -1)
			throw LauncherException("Problem with epoll_ctl ! 2");
	}
}

void	Launcher::process_writing_to_client(int i)
{
	std::vector< Client >::iterator it = this->_clients.begin();
	
	while (it != this->_clients.end())
	{
		if (it->getCom_socket() == _ep_event[i].data.fd)
			break ;
		++it;
	}
	if (it == this->_clients.end())
		throw LauncherException("Error: Client not found !");
	
	int			byte_already_sent = it->get_byte_sent();
	int 		byte_sent = 0;
	std::string	res_full = it->get_res_string();
	std::string res_remain = res_full.substr(byte_already_sent, res_full.size() - byte_already_sent);
	std::string	to_send;
	if (res_remain.size() <= BUFFER_SIZE)
		to_send = res_remain;
	else
		to_send = res_remain.substr(0, BUFFER_SIZE);
	byte_sent = send(it->getCom_socket(), to_send.c_str(), to_send.size(), 0);
	if (byte_sent != -1)
	{
		it->SetStatus(RECIVING_RES);
		it->set_byte_sent(byte_already_sent + byte_sent);
	}
	// I have sent everything to the client
	if (it->get_byte_sent() == static_cast<int>( res_full.size()))
	{
		it->SetStatus(RES_SENT);
		if (epoll_ctl(_efd, EPOLL_CTL_DEL, _ep_event[i].data.fd, NULL) == -1)
			throw LauncherException("Error: epoll_ctl DEL!");
		close(it->getCom_socket());
		std::cout << getTimestamp() <<  "\e[33m	I have sent the following response to client " << it->getId() << " : \e[32m " << std::endl;
		std::cout << std::endl << res_full << std::endl;
		std::cout << std::endl << "\e[33m	I will now remove client " << it->getId() << " from my client list.\e[0m" << std::endl;
		this->_clients.erase(it);
	}
}

void	Launcher::print_situation(void)
{
	char				str_ip_client[INET_ADDRSTRLEN];
	struct sockaddr_in  client_addr;
	
	std::cout << "--------------------- Servers -----------------" << std::endl; 
	for (std::vector<Server>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		std::cout << "Listen Socket : " << it->getListenSocket() << " | Name : " << it->getServerName()
		<< " | Host : " << it->getHost() << " | Port : " << ntohs(it->getPort()) << std::endl;
	}
	std::cout << "--------------------- Clients -----------------" << std::endl; 
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
	{
		client_addr = it->getClient_addr();
		inet_ntop (AF_INET, &(client_addr.sin_addr), str_ip_client, sizeof (str_ip_client));
		std::cout << "ID : " << it->getId() << "	|";
		std::cout << " Com Socket : " << it->getCom_socket() << "	|";
		std::cout << " Addresse : " << str_ip_client << "	|";
		std::cout << " Server :";
		it->print_ClientServer();
		std::cout << "	|";
		std::cout << " Status : ";
		switch (it->getStatus())
		{
		case 0:
			std::cout << "WANT_TO_SEND_REQ";
			break;
		case 1:
			std::cout << "SENDING_REQ";
			break;
		case 2:
			std::cout << "REQ_SENT";
			break;
		case 3:
			std::cout << "WAITING_FOR_RES";
			break;
		case 4:
			std::cout << "RECIVING_RES";
			break;
		default:
			std::cout << "RES_SENT";
			break;
		}
		std::cout << std::endl << std::endl;
	}
}
